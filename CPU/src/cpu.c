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

// El programa recibe la IP del nucleo como primer parametro
// y el puerto a conectarse como segundo parametro

int main(int argc,char **argv) {

	if (argc != 3) {
		perror("No se ingreso la cantidad de parametros correspondientes");
		return 1;
	}

	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
	serverAddress.sin_port = htons(argv[2]);

	int cliente = socket(AF_INET,SOCK_STREAM,0);
	if(connect(cliente,(void*)&serverAddress,sizeof(serverAddress)) != 0) {
		perror("No se pudo conectar");
		return 1;
	}

	while(1) {
		//Falta implementar que sucede cuando se conecta al nucleo
	}

	return EXIT_SUCCESS;

}
