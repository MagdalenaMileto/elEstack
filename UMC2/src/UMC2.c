/*
 ============================================================================
 Name        : UMC2.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "UMC2.h"


struct arg_struct {
	int socket;
};



int main(void) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */

    pthread_t thMOCK;
	          pthread_create(&thMOCK, NULL, hilo_mock, NULL);

	 int servidorSocket,socketCliente;
	     struct sockaddr_in addr;
	     socklen_t addrlen = sizeof(addr);
	     struct arg_struct *args;

	     servidorSocket=servidor(5002);

	     if(servidorSocket==-1){
	          printf("El AMEO NO ME DEJA CREAR SOCKET RE GATO\n");
	          close(servidorSocket);
	          exit(1);
	     }

	     listen(servidorSocket,5);

	     while(1){
	          socketCliente = accept(servidorSocket, (struct sockaddr *) &addr, &addrlen);

	          args = malloc(sizeof(struct arg_struct));//cuando se termine el proceso hacer un free de esto
	          args->socket=socketCliente;

	          pthread_t thCONEXION;
	          pthread_create(&thCONEXION, NULL, hilo_Conexion, (void *)args);

	          printf("Acepté cliente: %d\n",socketCliente);
	     }


	     if (abrirConfiguracion() == -1){
	    	 return -1;
	     }

///////////////////Conexion con Swap///////////////////
	     int clienteSwap;
	     clienteSwap = cliente(IP_SWAP,PUERTO_SWAP);

	     if(clienteSwap==0){
	    	 printf("UMC: No encontre SWAP me cierro. \n");
	    	 exit (EXIT_FAILURE);
	     }
	     printf("UMC: Conecté bien SWAP %d\n",clienteSwap);

///////////////////Conexion con Swap///////////////////

}


int abrirConfiguracion(){
	int error = 0;
	configuracion = config_create("config_umc.ini");
	printf("Cargando el archivo de configuracion. \n");

	if(config_has_property(configuracion, "PUERTO")){
		PUERTO = config_get_int_value(configuracion, "PUERTO");
		printf("Puerto detectado %d \n", PUERTO);
	}else{
		perror("No se encontro el puerto en el archivo de configuracion \n");
		error = 1;
	}

	if(config_has_property(configuracion, "IP_SWAP")){
		IP_SWAP = config_get_string_value(configuracion, "IP_SWAP");
		printf("La ip de swap es %s \n", IP_SWAP);
	}else{
		perror("No se encontro la IP del Swap en el archivo de configuracion \n");
		error = 1;
	}

	if(config_has_property(configuracion, "PUERTO_SWAP")){
		PUERTO_SWAP= config_get_int_value(configuracion, "PUERTO_SWAP");
		printf("El puerto de Swap es %d \n", PUERTO_SWAP);
	}else{
		perror("No se encontro el puerto del Swap en el archivo de configuracion \n");
		error = 1;
	}

	if(config_has_property(configuracion, "MARCOS")){
		MARCOS = config_get_int_value(configuracion, "MARCOS");
		printf("La cantidad de marcos disponilbes en el sistema es  %d \n", MARCOS);
	}else{
		perror("No se encontro la cantidad de marcos en el archivo de configuracion \n");
		error = 1;
	}

	if(config_has_property(configuracion, "MARCO_SIZE")){
		MARCO_SIZE = config_get_int_value(configuracion, "MARCO_SIZE");
		printf("El tamaño del marco es %d \n", MARCO_SIZE);

	}else{
		perror("No se encontro el tamaño de marco en el archivo de configuracion \n");
		error = 1;
	}

	if(config_has_property(configuracion, "MARCO_X_PROC")){
	//Le resto 1???//
//		int aux = config_get_int_value(configuracion, "MARCO_X_PROC");
//		MARCO_X_PROC = aux - 1;
		MARCO_X_PROC = config_get_int_value(configuracion, "MARCO_X_PROC");
		printf("La cantidad máxima de marcos por proceso es %d \n", MARCO_X_PROC);
	}else{
		perror("No se encontro la cantidad máxima de marcos por proceso en el archivo de configuracion \n");
		error = 1;
	}

	if(config_has_property(configuracion, "ALGORITMO")){
		ALGORITMO= config_get_string_value(configuracion, "ALGORITMO");
		printf("El algoritmo utilizado es %s \n", ALGORITMO);
	}else{
		perror("No se encontro el algortimo a utilizar \n");
		error = 1;
	}

	if(config_has_property(configuracion, "ENTRADAS_TLB")){
		ENTRADAS_TLB = config_get_int_value(configuracion, "ENTRADAS_TLB");
		printf("La cantidad de entradas en la TLB es  %d \n", ENTRADAS_TLB);
		if (ENTRADAS_TLB == 0){
			printf("Esta deshabilitada");
		}
	}else{
		perror("No se encontro la cantidad de entradas en tlb en el archivo de configuracion \n");
		error = 1;
	}

	if(config_has_property(configuracion, "RETARDO")){
		RETARDO = config_get_int_value(configuracion, "RETARDO");
		printf("El retardo es %d \n", RETARDO);
	}else{
		perror("No se encontro el retardo en el archivo de configuracion \n");
		error = 1;
	}

	if(error){
		return -1;
	}else{
		return 0;
	}
}

void *hilo_Conexion(void *arg){

    struct arg_struct *args = (struct arg_struct *)arg;
    int estado;

    while(1){

        t_header estructuraARecibir;
        sleep(1);

          estado=recibir_paquete(args->socket,&estructuraARecibir);

          	  //Podes hacer un switch aca

          if(estado==-1){  //Evento desconexion

               printf("Cerró Conexion %d\n",args->socket);
               //Deberia hacer todos los free del hilo
               break;

          }


          if(estado==101){									//Puedo enviar varios paquetes, PCB, handshake, etc. con esto identifico cual es
            	printf("Recibi %s\n",(char*)estructuraARecibir.data);

          }



          //Lo que recibi lo elimino
          estructuraARecibir.id=0; //Por las dudas, creo que no haria falta
          free(estructuraARecibir.data);

     }

     return 0;

}



/*
Inicializar programa

Parámetros: [Identificador del Programa] [Cantidad de Páginas totales requeridas] [Código del Programa]
Cuando el proceso Núcleo comunique el inicio de un nuevo Programa AnSISOP, se crearán las estructuras necesarias para administrarlo correctamente.
Para ello, UMC recibirá de este último un Identificador de programa, la cantidad de páginas totales requeridas, y el código del Programa.
Luego, deberá informar de esta situación al Proceso Swap, reservando la cantidad de páginas a usar, y escribiendo las páginas de código.
Notar que las páginas recibidas por la UMC no serán cargadas en memoria principal hasta que sea requerido, respetando el principio de paginación bajo demanda.
*/


//El char codigo se deberia cambiar por una estructura de programa, ej: prog->codigo
//int inicializarPrograma (int idProg, int cantPag, char codigo){
//	int envio;
//
//	envio = enviarNuevoProcesoASwap(idProg, cantPag, codigo);
//
//
//}

//int enviarNuevoProcesoASwap(int idProg, int cantPag, char codigo);
//{
//	int envio;
//
//	envio =	enviar_paquete()
//}


char mensaje[100]="HOLA QUE TAL\n";

void *hilo_mock(void *arg){

		sleep(1);
	int cliente2;
	cliente2 = cliente("127.0.0.1",5002);

	if(cliente2==0){
		printf("No encontre conexion me cierro :'( \n");
	  	exit (EXIT_FAILURE);
	}



	t_header header;

	header.id = 101;
	header.size = strlen(mensaje);
	header.data = mensaje;


	sleep(1);

	printf("MOCK:Envie %s\n",mensaje);
	enviar_paquete(cliente2, header);
	close(cliente2);
}

