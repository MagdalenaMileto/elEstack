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

char IP_SWAP[16];
int PUERTO_SWAP, PUERTO_NUCLEO, PUERTO_CPU;
int MARCOS,MARCO_SIZE;

typedef struct{
	char offset[MARCO_SIZE];
}t_pagina;

int tablaDePaginas[MARCOS];
int direccionesLogicas[MARCOS];

t_pagina *memoria = (t_pagina*)malloc(MARCO_SIZE*MARCOS);

int main(int argc,char **argv)
{

	if (argc != 2)
	{
		perror("No se paso la cantidad de parametros necesaria\n");
		return EXIT_FAILURE;
	}

	int nucleo,cpu,swap;

	leerArchivoDeConfiguracion();

	swap = cliente(IP_SWAP,PUERTO_SWAP);

	if (!swap)
	{
		printf("No se pudo abrir el socket a SWAP\n");
		return EXIT_FAILURE;
	}

	if (swap == -1)
	{
		printf("No se pudo conectar al SWAP\n");
		return EXIT_FAILURE;
	}

	nucleo = servidor(PUERTO_NUCLEO);
	cpu = servidor(PUERTO_CPU);

	int listenernucleo, listenercpu;	// Descriptor de socket a la escucha
	int nucleofd,cpufd,swapfd;	// Descriptor de socket de nueva conexión aceptada
	char buf[100];	// Buffer para datos del cliente
	int mensajeNucleo, mensajeCPU;
	int yes=1;	// Para setsockopt() SO_REUSEADDR, más abajo
	unsigned int addrlen = sizeof(struct sockaddr_in);

	// Gestionar las conexiones

	// Gestionar conexion del nucleo

	if ((nucleofd = accept(listenernucleo, (struct sockaddr*)&direccionNucleo, &addrlen)) == -1)
	{
		perror("accept");
	}
	else
		{

			printf("UMC: Nueva conexion desde %s en " "socket %d\n", inet_ntoa(direccionNucleo.sin_addr),nucleofd);

			// Hilo para el nucleo

			pthread_t hiloNucleo;
			char *arg1 = "thrn";
			int rNucleo;
			struct arg_struct *argsocket;
			argsocket = malloc(sizeof(struct arg_struct));
			argsocket->socket=nucleofd;

			rNucleo = pthread_create(&hiloNucleo,NULL,rutinaNucleo,(void *)argsocket);
			pthread_join( hiloNucleo, NULL);

		}


	// Gestionar conexion del cpu

	if ((cpufd = accept(listenercpu, (struct sockaddr*)&direccionCPU, &addrlen)) == -1)
	{
		perror("accept");
	}
	else
		{

		printf("UMC: Nueva conexion desde %s en " "socket %d\n", inet_ntoa(direccionCPU.sin_addr),cpufd);

	// Hilo para el cpu


		printf("UMC: Nueva conexion desde %s en " "socket %d\n", inet_ntoa(direccionNucleo.sin_addr),nucleofd);

		// Hilo para el nucleo

		pthread_t hiloCPU;
		char *arg1 = "thrcpu";
		int rCPU;
		struct arg_struct *argsocket;
		argsocket = malloc(sizeof(struct arg_struct));
		argsocket->socket=cpufd;

		rCPU = pthread_create(&hiloCPU,NULL,rutinaCPU,(void *)argsocket);
		pthread_join(hiloCPU, NULL);

		}

	return EXIT_SUCCESS;

}


