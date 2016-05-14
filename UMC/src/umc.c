/*
 * umc.c
 *
 *  Created on: 23 de abr. de 2016
 *      Author: Micaela Paredes
 */


#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>
#include<commons/config.h>
#include "../../COMUNES/handshake.h"
#include<pthread.h>
#define PORT 1992

int main(int argc,char **argv)
{
	if (argc != 5) {
		perror("No se ingreso la cantidad de parametros correspondientes");
		return EXIT_FAILURE;
	}


	t_config* configuracion;
	configuracion = config_create("Ruta al archivo de configuracion");

	char IP_SWAP[16];
	int PUERTO_SWAP, PUERTO_NUCLEO, PUERTO_CPU;
	int MARCOS;
	int MARCO_SIZE;

	if (config_has_property(configuracion,"IP_SWAP"))
		/* IPSWAP = config_get_string_value(configuracion,"IPSWAP");
else
{
	perror("No esta configurada la ip del swap");
	return 1;
}*/

		if (config_has_property(configuracion,"PUERTO_SWAP"))
			PUERTO_SWAP = config_get_int_value(configuracion,PUERTO_SWAP);
		else
		{
			perror("No esta configurado el puerto del swap");
			return 1;
		}

	if (config_has_property(configuracion,"PUERTO_NUCLEO"))
		PUERTO_NUCLEO = config_get_int_value(configuracion,PUERTO_NUCLEO);
	else
	{
		perror("No esta configurado el puerto del nucleo");
		return 1;
	}

	if (config_has_property(configuracion,"PUERTO_CPU"))
		PUERTO_CPU = config_get_int_value(configuracion,PUERTO_CPU);
	else
	{
		perror("No esta configurado el puerto del cpu");
		return 1;
	}

	int cantidadPaginas, tamanioPagina;
	typedef int tipoPaginaMemoria;
	// tipoPaginaMemoria memoria[MARCOS*MARCO_SIZE] = malloc(cantidadPaginas*tamanioPagina); // Memoria total
	int tablaDePaginas[cantidadPaginas];

	typedef struct{
		int offset[tamanioPagina];
	}t_pagina;

	typedef struct{
		int idPrograma;
		t_pagina pagina;
	}regPrograma;

	regPrograma direccionesLogicas[cantidadPaginas];

	void inicializarDireccionesLogicas(void){
		int i = 0;
		for(;i <= (cantidadPaginas - 1); i++){
			direccionesLogicas[i].idPrograma = 0;
			int c = 0;
			for(;c <= (tamanioPagina - 1); c++){
				direccionesLogicas[i].pagina.offset[c] = c;
			}
		}
	}

	void inicializarTablaDePaginas(void){
		int i = 0;
		for(;i<=cantidadPaginas; i++){
			tablaDePaginas[i] = 0;
		}

		void inicializarPrograma(int idProg, int paginasRequeridas)
		{
			int c = 0;
			while(direccionesLogicas[c].idPrograma == 0){
				c++;
			}
			int limite = c + paginasRequeridas;
			for(; c<= limite; c++)
			{
				direccionesLogicas[c].idPrograma = idProg;
			}
			// void informarInicializacionASwap(int paginasRequeridas){}
			sleep(1);
		}

		/*	void almacenarBytes(int pagina, int offset, int tamanioBuffer, int buffer[tamanioBuffer]){

	}

	void solicitarBytes(int pagina, int offset, int cantidadBytes){

		}

	void cambioDeProcesoActivo(int idProg){
	}

// Conexion con el nucleo dice que finalice programa

    pthread_t hiloNucleo;
    char *arg1 = "thrn";
    int rNucleo;

	rNucleo = pthread_create( &hiloNucleo, NULL, rutinaNucleo, (nucleofd) arg1);

	pthread_join( hiloNucleo, NULL);

	void finalizarPrograma(int idProg){

		// falta terminar
		// void informarFinalizacionASwap(int idProg){}

		 sleep(1);
	}

		 */


		const int nucleoport = 1200;
		const int cpuport = 1205;

		struct sockaddr_in direccionNucleo;
		direccionNucleo.sin_family = AF_INET;
		direccionNucleo.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		direccionNucleo.sin_port = htons(nucleoport);

		struct sockaddr_in direccionCPU;
		direccionCPU.sin_family = AF_INET;
		direccionCPU.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		direccionCPU.sin_port = htons(cpuport);


		struct sockaddr_in direccionSwap;
		direccionSwap.sin_family = AF_INET;
		direccionSwap.sin_addr.s_addr = inet_addr(argv[1]);
		direccionSwap.sin_port = htons(atoi(argv[2]));


		int listenernucleo, listenercpu;	// Descriptor de socket a la escucha
		int nucleofd,cpufd,swapfd;	// Descriptor de socket de nueva conexión aceptada
		char buf[100];	// Buffer para datos del cliente
		int mensajeNucleo, mensajeCPU;
		int yes=1;	// Para setsockopt() SO_REUSEADDR, más abajo
		unsigned int addrlen = sizeof(struct sockaddr_in);

		FD_ZERO(&read_fds);	// Obtener socket a la escucha

		if ((listenernucleo = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			perror("socket");
			exit(1);
		}

		if ((listenercpu = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			perror("socket");
			exit(1);
		}

		if ((swapfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			perror("socket");
			exit(1);
		}

		// Obviar el mensaje "address already in use"
		if (setsockopt(listenernucleo, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (setsockopt(listenercpu, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (setsockopt(swapfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}


		if (bind(listenernucleo, (struct sockaddr*)&direccionNucleo, sizeof(direccionNucleo)) == -1) {
			perror("bind");

			exit(1);
		}

		if (bind(listenercpu, (struct sockaddr*)&direccionCPU, sizeof(direccionCPU)) == -1) {
			perror("bind");

			exit(1);
		}

		// Escuchar
		if (listen(listenernucleo, 10) == -1) {
			perror("listen");
			exit(1);
		}

		if (listen(listenercpu, 10) == -1) {
			perror("listen");
			exit(1);
		}


		if(connect(swapfd,(void*)&direccionSwap,sizeof(direccionSwap)) != 0) {
			perror("No se pudo conectar al swap.");
		}



		// Gestionar las conexiones

		// Gestionar conexion del nucleo

		if ((nucleofd = accept(listenernucleo, (struct sockaddr*)&direccionNucleo, &addrlen)) == -1)
		{
			perror("accept");
		} else {
			// Conexion con el nucleo dice que finalice programa

			pthread_t hiloNucleo;
			char *arg1 = "thrn";
			int rNucleo;

			rNucleo = pthread_create( &hiloNucleo, NULL, rutinaNucleo, (nucleofd) arg1);

			pthread_join( hiloNucleo, NULL);

		}
		printf("UMC: Nueva conexion desde %s en "
				"socket %d\n", inet_ntoa(direccionNucleo.sin_addr),nucleofd);
	}

	// Gestionar conexion del cpu

	if ((cpufd = accept(listenercpu, (struct sockaddr*)&direccionCPU, &addrlen)) == -1)
	{
		perror("accept");
	}
	printf("UMC: Nueva conexion desde %s en "
			"socket %d\n", inet_ntoa(direccionCPU.sin_addr),cpufd);



return EXIT_SUCCESS;

}


