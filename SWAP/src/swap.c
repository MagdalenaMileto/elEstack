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
#include <netinet/in.h>
#include<netinet/ip.h>
#include<unistd.h>
#define Puerto 1206


int main(int argc,char **argv) {
	printf("SWAP: INICIO\n");
//
//	struct sockaddr_in umcAddress;
//		umcAddress.sin_family = AF_INET;
//		umcAddress.sin_addr.s_addr = inet_addr(argv[1]);
//		umcAddress.sin_port = htons(atoi(argv[2]));


//
//		int umc = socket(AF_INET,SOCK_STREAM,0);
//			if(connect(umc,(void*)&umcAddress,sizeof(umcAddress)) != 0) {
//				perror("No se pudo conectar al nucleo.");
//				return EXIT_FAILURE;
//			}
//		close(umc);


	int sock_lst, new_lst;  // Escuchar sobre sock_lst, nuevas conexiones sobre new_lst
	struct sockaddr_in my_addr;    // información sobre mi dirección
	struct sockaddr_in umcAddress; // información sobre la dirección del cliente
	my_addr.sin_family = AF_INET;         // Ordenación de bytes de máquina
		my_addr.sin_port = htons(Puerto);     // short, Ordenación de bytes de la red
		my_addr.sin_addr.s_addr = INADDR_ANY; // rellenar con mi dirección IP
		memset(&(my_addr.sin_zero), '\0', 8); // poner a cero el resto de la estructura
	int yes=1;
	int mensajeUmc;
	char buffer[100];
	unsigned int sin_size = sizeof(struct sockaddr_in);

	if ((sock_lst = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	if (setsockopt(sock_lst,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
	    perror("setsockopt");
	    exit(1);
	}


	if (bind(sock_lst, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		exit(1);
	}

	if (listen(sock_lst, 10) == -1) {
	            perror("listen");
	            exit(1);
	}

	if ((new_lst = accept(sock_lst, (struct sockaddr *)&umcAddress, &sin_size)) == -1)
	{
		perror("accept");
	}
	else {
        printf("SWAP: Nueva conexion desde %s en" "socket %d\n",inet_ntoa(umcAddress.sin_addr), new_lst);
	}

	if ((mensajeUmc = recv(new_lst, buffer, sizeof(buffer), 0)) <= 0) {
		if (mensajeUmc == 0) {
			// Conexión cerrada
			printf("SWAP: Select: La UMC %d se ha desconectado\n", new_lst);
			} else {
				perror("recv");
			}
			//printf("SWAP: El mensaje:  %s\n de la UMC a llegado al Swap.\n",buffer);


		}
		printf("SWAP: El mensaje:  %s de la UMC a llegado al Swap.\n",buffer);

		printf("SWAP: me cierro\n");
	return EXIT_SUCCESS;
}

