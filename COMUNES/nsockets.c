#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include "nsockets.h"
#include <errno.h>


#define SOCKET_ERROR -1


//Librerar datos

signed int cliente(char *ip_server, int puerto)
{
	int iSocket; 					// Escuchar sobre sock_fd, nuevas conexiones sobre new_fd
	struct sockaddr_in their_addr; 	// Información sobre mi dirección




    struct timeval  timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    fd_set set;
   
    int so_error;
    socklen_t len;

   


	// Seteo IP y Puerto
	their_addr.sin_family = AF_INET;  					// Ordenación de bytes de la máquina
	their_addr.sin_port = htons(puerto); 				// short, Ordenación de bytes de la red
	their_addr.sin_addr.s_addr = inet_addr(ip_server);
	memset(&(their_addr.sin_zero), '\0', 8); 			// Poner a cero el resto de la estructura



	// Pido socket
	if ((iSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		//log_error(logger, "socket: %s", strerror(errno));
		perror("NSOCKETS: No se pudo conectar cliente\n");
		return 0;
	}

  
	fcntl(iSocket, F_SETFL, O_NONBLOCK);



	// Intento conectar
	if (connect(iSocket, (struct sockaddr *) &their_addr, sizeof their_addr) == -1) {
		//log_error(logger, "connect: %s", strerror(errno));
		//puts("error");

		//return 0;
	}
	//return iSocket;
	FD_ZERO(&set);
    FD_SET(iSocket, &set);
	select(iSocket+1, NULL, &set, NULL, &timeout);

	len = sizeof(so_error);
	getsockopt(iSocket, SOL_SOCKET, SO_ERROR, &so_error, &len);

	if(so_error!=0){return 0;} 
	//puts(&status);
    return (iSocket);



}


/*
* Abre un socket servidor de tipo AF_INET. Devuelve el descriptor
* del socket o -1 si hay probleamas
*/

int32_t servidor (int puerto){
	struct sockaddr_in direccion;
	int descriptor;

	/*
	* se abre el socket
	*/
	descriptor = socket (AF_INET, SOCK_STREAM, 0);
	if (descriptor == SOCKET_ERROR)
	 	return SOCKET_ERROR;

	/*
	* Se rellenan los campos de la estructura Direccion, necesaria
	* para la llamada a la funcion bind()
	*/
	direccion.sin_family = AF_INET;
	direccion.sin_port = htons(puerto);
	//direccion.sin_addr.s_addr =INADDR_ANY;
	//Se prueba usando LOCALHOST : 127.0.0.1
	direccion.sin_addr.s_addr = inet_addr("127.0.0.1");

int enable = 1;
if (setsockopt(descriptor, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    perror("setsockopt(SO_REUSEADDR) failed");


	if (bind (descriptor, (struct sockaddr *)&direccion, sizeof(direccion)) == SOCKET_ERROR){
		perror("NSOCKETS: No se pudo blindear\n");
		
		close (descriptor);
		return SOCKET_ERROR;
	}

	/*
	* Se avisa al sistema que comience a atender llamadas de clientes
	*/
	if (listen (descriptor, 1) == SOCKET_ERROR){
		close (descriptor);
		return SOCKET_ERROR;
	}

	/*
	* Se devuelve el descriptor del socket servidor
	*/
	return descriptor;
}





/******************






lectura y escritura



*/




int32_t leer_socket (int32_t nuevo_socket, char *buffer, size_t size)
{
	size_t leido = 0;
	size_t aux = 0;

	/*Comprobacion de que los parametros de entrada son correctos*/

	if ((nuevo_socket == -1) || (buffer == NULL) || (size < 1))
		return -1;

	/* Mientras no hayamos leido todos los datos solicitados*/
	while (leido < size){
		aux = recv(nuevo_socket, buffer + leido, size - leido, 0);

		if (aux > 0){
			/*
			* En caso de leer datos, se incrementa la variable que contiene los datos leidos hasta el momento
			*/
			leido = leido + aux;
		}
		else{
			/*
			* Si read devuelve 0, es que se cerro el socket. Se devuelven los caracteres leidos hasta ese momento
			*/
			if (aux == 0)
			{
				return leido;
			}
			if (aux == -1){
				/*
				* En caso de error:
				* EINTR se produce hubo una  interrupcion del sistema antes de leer ningun dato. No
				* es un error realmente.
				* EGAIN significa que el socket no esta disponible por el  momento.
				* Ambos errores se tratan con una espera de 100 microsegundos y se vuelve a intentar.
				* El resto de los posibles errores provocan que salgamos de la funcion con error.
				*/

				//Mover la logica de señales al Sig_handler

				switch (errno){
					case EINTR:
					case EAGAIN:
						usleep (100);
						break;
					default:
						//Se desconecto el Socket
						return -1;
				}
			}
		}
	}
	/*
	* Se devuelve el total de los caracteres leidos
	*/
	return leido;
}

/*
* Escribe dato en el socket cliente. Devuelve numero de bytes escritos, o -1 si hay error.
*/
int32_t escribir_socket (int32_t nuevo_socket, char *datos, size_t longitud)
{
	size_t escrito = 0;
	size_t aux = 0;

	/*
	* Comprobacion de los parametros de entrada
	*/
	if ((nuevo_socket == -1) || (datos == NULL) || (longitud < 1))
		return -1;

	/*
	* Bucle hasta que hayamos escrito todos los caracteres que se indicaron.
	*/
	while (escrito < longitud){
		aux = send(nuevo_socket, datos + escrito, longitud - escrito, 0);
		if (aux > 0){
			/*
			* Si se consiguio escribir caracteres, se actualiza la variable escrito
			*/
			escrito = escrito + aux;
		}
		else
		{
			/*
			* Si se cerro el socket, devolvemos el numero de caracteres leidos.
			* Si hubo un error, devuelve -1
			*/
			if (aux == 0){
				return escrito;
			}
			else
			{
				return -1;
			}
		}
	}

	/*
	* Devolvemos el total de caracteres leidos
	*/
	return escrito;
}


/*Para recibir un paquete, se pasa como parametro el socket en el que se va esperar, un puntero a un tipo header
 * y uno data para que reciban ahi los datos junto con un identificador para ver que tipo de estructura es ,
 * ya que se utilizara como buffer para retornar lo que se reciba a travez
 * del socket. Si falla devuelve un valor negativo*/

int32_t recibir_paquete(int32_t enlace,t_header* header_a_recibir)
{

	int32_t res=1;	//Me aseguro que recibi los datos con 1
	char* buffer;
	size_t size_t_header;

	size_t_header = sizeof(t_header);
	buffer = malloc(size_t_header);
	leer_socket(enlace, buffer, size_t_header);
	memcpy(header_a_recibir, buffer, size_t_header);
	//printf("%d\n",header_a_recibir->size);
	free(buffer);
	buffer = NULL;

	if(header_a_recibir->size > 0){
		buffer = malloc(header_a_recibir->size);
		if(leer_socket(enlace, buffer,header_a_recibir->size) == header_a_recibir->size)
		{
			header_a_recibir->data = malloc(header_a_recibir->size);
			memcpy(header_a_recibir->data,buffer,header_a_recibir->size);
		}
		else
		{
			res = -1;	//En caso de que no reciba nada
		}
	}
	free(buffer);
	buffer = NULL;

	return res;
}

/*Para enviar un paquete, se pasa como parametro a quien va dirigido, junto con el tipo header y data que se van a enviar
 * Si falla devuelve un valor negativo*/
int32_t enviar_paquete(int32_t enlace,t_header header_a_enviar)
{

	int32_t res;
	size_t size_t_header;

	size_t_header = sizeof(t_header);

	if (enlace != 0)
	{
		res = escribir_socket(enlace,(char*)&header_a_enviar, size_t_header);
		if(res != size_t_header)
		{
			return -1;
		}
		if(header_a_enviar.size != 0)
		{
			res = escribir_socket(enlace,header_a_enviar.data,header_a_enviar.size);

			if(res != header_a_enviar.size)
			{
				return -1;
			}
		}
	}
	else
	{
		res = -1;
	}


	return res; 		//Devuelvo el size de lo enviado
}







/*




*/



/*
* Se le pasa un socket de servidor y acepta en el una conexion de cliente.
* devuelve el descriptor del socket del cliente o -1 si hay problemas.
*/
int32_t aceptar_cliente (int descriptor){

	socklen_t longitud_cliente;
	struct sockaddr cliente;
	int32_t hijo;

		/*
		* La llamada a la funcion accept requiere que el parametro
		* longitud_cliente contenga inicialmente el tamano de la
		* estructura Cliente que se le pase. A la vuelta de la
		* funcion, esta variable contiene la longitud de la informacion
		* util devuelta en Cliente
		*/
	longitud_cliente = sizeof(cliente);
	hijo = accept (descriptor, &cliente, &longitud_cliente);
	if (hijo == SOCKET_ERROR)
		return SOCKET_ERROR;

		/*
		* Se devuelve el descriptor en el que esta "enchufado" el cliente.
		*/
	return hijo;
}

//Lo libera inmediatamente despues de cerrar el socket
int32_t cerrar_socket(int descriptor){

	if (close(descriptor) == SOCKET_ERROR)
		return SOCKET_ERROR;

	return EXIT_SUCCESS;
}

int32_t liberar_puerto (int descriptor){

	int optval = 1;
	if (setsockopt(descriptor, SOL_SOCKET, SO_REUSEADDR, &optval,sizeof optval) == -1)
		return SOCKET_ERROR;

	return EXIT_SUCCESS;
}

int32_t set_nonblocking(int descriptor){
	if (fcntl(descriptor, F_SETFL, O_NONBLOCK) == -1)
		return SOCKET_ERROR;
	return EXIT_SUCCESS;
}





