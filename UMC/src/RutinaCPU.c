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
// Gestionar rutina del nucleo

void rutinaNucleo(int cpufd)
{
	int buf;
	int bytesRecibidos;
	if ((bytesRecibidos = recv(cpufd, &buf, sizeof(buf), 0)) <= 0)
	{

		// Error o conexión cerrada por el cliente
		if (bytesRecibidos == 0)
		{

			// Conexión cerrada
			printf("UMC: Select: El cpu %d se ha desconectado\n", cpufd);
		} else
			perror("recv");
	}
		bzero(&buf,sizeof(buf));            // Vaciar buffer
	}

