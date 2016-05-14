/*
 * swap.c
 *
 *  Created on: 21/4/2016
 *      Author: Ivan Bober
 */

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<unistd.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/txt.h>
#include<sys/mman.h>


#define Puerto 1206

	char* Nombre_Swap;
	int Puerto_Swap;
	int Cantidad_Paginas;
	int Tamanio_Pagina;
	int Retardo_Compactacion;
	t_config* configuracion;
	long int tamanio_archivo;
	char * discoSwapMapped;

	int	abrirConfiguracion();
	int	crearArchivo();
	int mapearArchivo();
	int iniciarServidor();
	int establecerConexion(int sock_lst);

	typedef struct __attribute__((packed)){
		int pid;
		int nroPag;
		int posicion;
		char texto[1024];
	} paqueteUMC;

int main(int argc,char **argv) {

	int sock_lst;
	int new_lst;
	int mensajeUmc;
	char buffer[100];

	abrirConfiguracion();
	crearArchivo();
	mapearArchivo();

	sock_lst = iniciarServidor();
	new_lst = establecerConexion(sock_lst);

	if ((mensajeUmc = recv(new_lst, buffer, sizeof(buffer), 0)) <= 0) {
		if (mensajeUmc == 0) {
			// UMC Cerro la conexion
			printf("SWAP: Select: La UMC %d se ha desconectado\n", new_lst);
			return EXIT_FAILURE;
		} else {
			//Hubo un error en la conexion
			perror("recv");
			return EXIT_FAILURE;
		}
	}
	printf("SWAP: El mensaje:  %s\n de la UMC a llegado al Swap.\n",buffer);

	//Me llega un nuevo proceso
	printf("LLego nuevo proceso al SWAP \n");


	printf("SWAP: me cierro\n");
	return EXIT_SUCCESS;
}

int abrirConfiguracion() {
	int error = 0;
	configuracion = config_create("config_swap.ini");
	printf("Cargando el archivo de configuracion. \n");
	if (configuracion->properties->elements_amount <= 0) {
		perror("No se pudo cargar el archivo de configuración.");
		return -1;
	}

	if(config_has_property(configuracion, "Puerto_Escucha")){
		Puerto_Swap = config_get_int_value(configuracion, "Puerto_Escucha");
		printf("El puerto habilitado es el %d \n", Puerto_Swap);
	}else{
		perror("No se encontro el puerto en el archivo de configuracion");
		error = 1;
	}

	if(config_has_property(configuracion, "Nombre_Swap")){
		Nombre_Swap = config_get_string_value(configuracion, "Nombre_Swap");
		printf("El archivo en disco se llama: %s \n", Nombre_Swap);
	}else{
		perror("No se encontro el nombre del archivo en disco en el archivo de configuracion");
		error = 1;
	}

	if(config_has_property(configuracion, "Cantidad_Paginas")){
		Cantidad_Paginas = config_get_int_value(configuracion, "Cantidad_Paginas");
		printf("El archivo en disco contiene %d paginas\n", Cantidad_Paginas);
	}else{
		perror("No se encontro la cantidad de paginas en el archivo de configuracion");
		error = 1;
	}
	if(config_has_property(configuracion, "Tamanio_Pagina")){
		Tamanio_Pagina = config_get_int_value(configuracion, "Tamanio_Pagina");
		printf("El tamanio de cada pagina es de %d \n", Tamanio_Pagina);
	}else{
			perror("No se encontro el tamanio de paginas en el archivo de configuracion");
			error=1;
	}
		if(config_has_property(configuracion, "Retardo_Compactacion")){
			Retardo_Compactacion = config_get_int_value(configuracion, "Retardo_Compactacion");
			printf("El tiempo de retardo en compactacion, es de: %d \n", Retardo_Compactacion);
		}else{
			perror("No se encontro el retardo de compactacion en el archivo de configuracion");
			error=1;
		}
	if(error){
		return -1;
	}else{
		return 0;
	}
}

int crearArchivo(){
	Nombre_Swap = config_get_string_value(configuracion, "Nombre_Swap");
	printf("El archivo en disco se llama: %s \n", Nombre_Swap);
	FILE *arch = fopen(Nombre_Swap,"w");
	tamanio_archivo = Cantidad_Paginas*Tamanio_Pagina;
	char *vectorArchivo[tamanio_archivo];
	int i;
	char* caracter = "\0";
	for(i=0;i<tamanio_archivo;i++){
		vectorArchivo[i] = caracter;
	}
	for(i=0;i<tamanio_archivo;i++){
		fwrite(vectorArchivo[i],1,1,arch);
	}

	fclose(arch);

	return 1;
}

int mapearArchivo(){
	int fd;
	size_t length = tamanio_archivo;
	discoSwapMapped=malloc(tamanio_archivo);
	fd = open(Nombre_Swap, O_RDONLY);
	  if (fd == -1) {
		printf("Error abriendo %s para su lectura", Nombre_Swap);
		exit(EXIT_FAILURE);
	    }
	  discoSwapMapped = mmap(0, length,PROT_READ, MAP_SHARED,fd,0);
	if (discoSwapMapped == MAP_FAILED) {
		close(fd);
		printf("Error mapeando el archivo %s \n", Nombre_Swap);
		exit(EXIT_FAILURE);
	    }
	printf("Mapeo perfecto  %s \n", Nombre_Swap);
	return 1;
}

int iniciarServidor(int puerto) {

	int sock_lst;  // Escuchar sobre sock_lst, nuevas conexiones sobre new_lst
	struct sockaddr_in my_addr;    // información sobre mi dirección
	my_addr.sin_family = AF_INET;         // Ordenación de bytes de máquina
	my_addr.sin_port = htons(Puerto);     // short, Ordenación de bytes de la red
	my_addr.sin_addr.s_addr = INADDR_ANY; // rellenar con mi dirección IP
	memset(&(my_addr.sin_zero), '\0', 8); // poner a cero el resto de la estructura
	int yes=1;

	if ((sock_lst = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return -1;
	}

	if (setsockopt(sock_lst,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		perror("setsockopt");
		return -1;
	}

	if (bind(sock_lst, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		return -1;
	}

	if (listen(sock_lst, 10) == -1) {
		perror("listen");
		return -1;
	}

	return sock_lst;
}

int establecerConexion(int sock_lst) {
	int new_lst;
	struct sockaddr_in umcAddress; // información sobre la dirección del cliente
	unsigned int sin_size = sizeof(struct sockaddr_in);
	if ((new_lst = accept(sock_lst, (struct sockaddr *)&umcAddress, &sin_size)) == -1)
	{
		perror("accept");
	}
	else {
        printf("SWAP: Nueva conexion desde %s en" "socket %d\n",inet_ntoa(umcAddress.sin_addr), new_lst);
        return -1;
	}
	return new_lst;
}
