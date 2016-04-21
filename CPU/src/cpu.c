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

	struct sockaddr_in nucleoAddress;
	nucleoAddress.sin_family = AF_INET;
	nucleoAddress.sin_addr.s_addr = inet_addr(argv[1]);
	nucleoAddress.sin_port = htons(atoi(argv[2]));

	struct sockaddr_in umcAddress;
	umcAddress.sin_family = AF_INET;
	umcAddress.sin_addr.s_addr = inet_addr(argv[3]);
	umcAddress.sin_port = htons(atoi(argv[4]));

	int nucleo = socket(AF_INET,SOCK_STREAM,0);
	if(connect(nucleo,(void*)&nucleoAddress,sizeof(nucleoAddress)) != 0) {
		perror("No se pudo conectar al nucleo.");
		return EXIT_FAILURE;
	}

	int umc = socket(AF_INET,SOCK_STREAM,0);
	if(connect(umc,(void*)&nucleoAddress,sizeof(nucleoAddress)) != 0) {
		perror("No se pudo conectar a la umc.");
		return EXIT_FAILURE;
	}

	while(1) {
		//Falta implementar que sucede cuando se conecta al nucleo y a la umc
	}

	close(nucleo);
	close(umc);
	return EXIT_SUCCESS;

}
