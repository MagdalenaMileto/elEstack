/*
 * consola.c
 *
 *  Created on: 23/4/2016
 *      Author: utnso
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(void) {
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr("127.0.0.1");
	direccionServidor.sin_port = htons(1201);

	int cliente = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(cliente, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("No se pudo conectar");
		return 1;
	}

	//while (1) {
		char mensaje[100]="Entrega 30/4 \n";
		//scanf("%s\n", mensaje);

		send(cliente, mensaje, sizeof(char)*100, 0);

		puts("CONSOLA: envio");
		
	//}

	return 0;
}

