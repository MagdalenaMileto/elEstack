/*
 * cpu.c
 *
 *  Created on: 20 de abr. de 2016
 *      Author: Franco Albornoz
 */


#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include <netinet/in.h>
#include<netinet/ip.h>
#include<unistd.h>

/* El programa recibe la IP del nucleo como primer y segundo parametros
 * la direccion y puerto del nucleo y como tercer y cuarto parametros
 * la direccion y puerto de la umc.
 */

int main(int argc,char **argv) {

	if (argc != 5) {
		perror("No se ingreso la cantidad de parametros correspondientes");
		return EXIT_FAILURE;
	}

	int maxfd = 3;				// Indice de maximo FD
	char buffer[100];			// Bufer para send/recv

	struct timeval tv;			// Estructura para select()
	tv.tv_sec = 2;
	tv.tv_usec = 500000;

	fd_set readfds,masterfds;	// Estructuras para select()
	FD_ZERO(&readfds);

	struct sockaddr_in nucleoAddress;
	nucleoAddress.sin_family = AF_INET;
	nucleoAddress.sin_addr.s_addr = inet_addr(argv[1]);
	nucleoAddress.sin_port = htons(atoi(argv[2]));

	struct sockaddr_in umcAddress;
	umcAddress.sin_family = AF_INET;
	umcAddress.sin_addr.s_addr = inet_addr(argv[3]);
	umcAddress.sin_port = htons(atoi(argv[4]));

	int nucleo = socket(AF_INET,SOCK_STREAM,0);
	if (nucleo>maxfd)
		maxfd = nucleo;			// Se actualiza el indice de maximo fd
	if(connect(nucleo,(void*)&nucleoAddress,sizeof(nucleoAddress)) != 0) {
		perror("No se pudo conectar al nucleo.");
		return EXIT_FAILURE;
	}
	FD_SET(nucleo,&masterfds);	// Se agrega socket a la lista de fds

	int umc = socket(AF_INET,SOCK_STREAM,0);
	if (umc>maxfd)
		maxfd = umc;			// Se actualiza el indice de maximo fd
	if(connect(umc,(void*)&umcAddress,sizeof(umcAddress)) != 0) {
		perror("No se pudo conectar a la umc.");
		return EXIT_FAILURE;
	}
	FD_SET(umc,&masterfds);		// Se agrega socket a la lista de fds

	while(1) {
		readfds = masterfds;	// Copio el struct con fds al auxiliar para read
		select(maxfd+1,&readfds,NULL,NULL,&tv);
		if (FD_ISSET(nucleo, &readfds))		// Si el nucleo envio algo
		{
			recv(nucleo,buffer,100,0);
			printf("El nucleo informa lo siguiente: %s\nMensaje enviado a la UMC.\n",buffer);
			send(umc,buffer,100,0);

		}
	}

	close(nucleo);
	close(umc);
	return EXIT_SUCCESS;

}
