/*
 * RutinaCPU.c
 *
 *  Created on: 14/5/2016
 *      Author: Micaela Paredes
 */

#include<sys/socket.h>
#include<stdio.h>
#include<stdio.h>
#include<strings.h>

typedef struct arg_sockets{
	int socket;
}arg_sockets;

// Gestionar rutina del nucleo

*rutinaCPU(void *argumentoSocket)
{
	int buf;
	int bytesRecibidos;
	arg_sockets *argumentos = (arg_sockets *)argumentoSocket;;

	if ((bytesRecibidos = recv(argumentos->socket, &buf, sizeof(buf), 0)) <= 0)
	{

		// Error o conexión cerrada por el cliente
		if (bytesRecibidos == 0)
		{

			// Conexión cerrada
			printf("UMC: Select: El nucleo %d se ha desconectado\n", argumentos->socket);
		} else
			perror("recv");
	}
		bzero(&buf,sizeof(buf));            // Vaciar buffer
}
