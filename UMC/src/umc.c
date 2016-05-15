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
int cantidadPaginas, tamanioPagina;

int main(int argc,char **argv)
{
	if (argc != 5)
	{
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


	if ((listenernucleo = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}

	if ((listenercpu = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
			perror("socket");
			exit(1);
	}

	if ((swapfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
			perror("socket");
			exit(1);
	}

	// Obviar el mensaje "address already in use"
	if (setsockopt(listenernucleo, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1)
	{
		perror("setsockopt");
		exit(1);
	}

	if (setsockopt(listenercpu, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1)
	{
		perror("setsockopt");
		exit(1);
	}

	if (setsockopt(swapfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1)
	{
		perror("setsockopt");
		exit(1);
	}


	if (bind(listenernucleo, (struct sockaddr*)&direccionNucleo, sizeof(direccionNucleo)) == -1)
	{
		perror("bind");

		exit(1);
	}

	if (bind(listenercpu, (struct sockaddr*)&direccionCPU, sizeof(direccionCPU)) == -1)
	{
		perror("bind");

		exit(1);
	}

	// Escuchar
	if (listen(listenernucleo, 10) == -1)
	{
		perror("listen");
		exit(1);
	}

	if (listen(listenercpu, 10) == -1)
	{
		perror("listen");
		exit(1);
	}


	if(connect(swapfd,(void*)&direccionSwap,sizeof(direccionSwap)) != 0)
	{
		perror("No se pudo conectar al swap.");
	}


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


