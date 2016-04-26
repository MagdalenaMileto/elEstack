/*
 * umc.c
 *
 *  Created on: 23 de abr. de 2016
 *      Author: Micaela Paredes
 */


#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(1992);

	int servidor = socket(AF_INET, SOCK_STREAM, 0);
	int activado = 1;

	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if (bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("Falló el bind");
		return 1;
		}

	printf("Estoy escuchando\n");
	listen(servidor, 100);

	struct sockaddr_in direccionCliente;
	unsigned int len;
	int cliente = accept(servidor, (void*) &direccionCliente, &len);

	printf("Recibí una conexión en %d!!\n", cliente);
	send(cliente, "Hola cliente :D!", 16, 0);

	char* buffer = malloc(5);

	int bytesRecibidos = recv(cliente, buffer, 4, MSG_WAITALL);
	if (bytesRecibidos < 0) {
		perror("Se ha desconectado, hubo un error.");
		return 1;
		}

	buffer[bytesRecibidos] = '\0';
	printf("Me llegaron %d bytes con %s", bytesRecibidos, buffer);

	free(buffer);

	close(servidor);

	return 0;


}
