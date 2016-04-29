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
#define PORT 1992


// Puerto para escuchar
int main(int argc,char **argv)
{
	if (argc != 3) {
			perror("No se ingreso la cantidad de parametros correspondientes");
			return EXIT_FAILURE;
		}

	fd_set master; 	// Conjunto maestro de descriptores de fichero
	fd_set read_fds; // Conjunto temporal de descriptores de fichero

	const int nucleoport = 1200;
	const int cpuport = 1203;

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



	int fdmax = 3;	// Número máximo de descriptores de fichero
	int listenernucleo, listenercpu;	// Descriptor de socket a la escucha
	int nucleofd,cpufd,swapfd;	// Descriptor de socket de nueva conexión aceptada
	char buf[256];	// Buffer para datos del cliente
	int mensajeNucleo, mensajeCPU;
	int yes=1;	// Para setsockopt() SO_REUSEADDR, más abajo
	unsigned int addrlen = sizeof(struct sockaddr_in);
	FD_ZERO(&master);	// Borra los conjuntos maestro y temporal
	FD_ZERO(&read_fds);	// Obtener socket a la escucha

	if ((listenernucleo = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	if ((listenercpu = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	if ((swapfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	// Obviar el mensaje "address already in use"
	if (setsockopt(listenernucleo, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	if (setsockopt(listenercpu, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	if (setsockopt(swapfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

//	memset(&(direccionServidor.sin_zero), '\0', 8);
	if (bind(listenernucleo, (struct sockaddr*)&direccionNucleo, sizeof(direccionNucleo)) == -1) {
		perror("bind");

		exit(1);
	}

	if (bind(listenercpu, (struct sockaddr*)&direccionCPU, sizeof(direccionCPU)) == -1) {
		perror("bind");

		exit(1);
	}

	// Escuchar
	if (listen(listenernucleo, 10) == -1) {
		perror("listen");
		exit(1);
	}

	if (listen(listenercpu, 10) == -1) {
		perror("listen");
		exit(1);
	}

	// Añadir listener al conjunto maestro
	FD_SET(listenernucleo, &master);
	FD_SET(listenercpu, &master);

	// Seguir la pista del descriptor de fichero mayor
	if (listenernucleo > fdmax)
	{
		if (listenernucleo > listenercpu)
			fdmax = listenernucleo;
		else
			fdmax = listenercpu;
	}

	// Bucle principal, todavía no es necesario
/*	for(;;) {
		read_fds = master;
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(1);
		}  */

	swapfd = socket(AF_INET,SOCK_STREAM,0);
		if (swapfd>fdmax)
			fdmax = swapfd;			// Actualizar el maximo
		if(connect(swapfd,(void*)&direccionSwap,sizeof(direccionSwap)) != 0) {
			perror("No se pudo conectar al swap.");
			return EXIT_FAILURE;
		}

		FD_SET(swapfd,&master);		// Añadir swap al conjunto maestro


	// Gestionar las conexiones
				// Gestionar conexion del nucleo

					if ((nucleofd = accept(listenernucleo, (struct sockaddr*)&direccionNucleo, &addrlen)) == -1)
					{
						perror("accept");
					} else {
						FD_SET(nucleofd, &master);  // Añadir nucleo al conjunto maestro
						if (nucleofd > fdmax) {    	// Actualizar el máximo
							fdmax = nucleofd;
						}
						printf("Nueva conexion desde %s en "
								"socket %d\n", inet_ntoa(direccionNucleo.sin_addr),nucleofd);
					}


					// Gestionar datos del nucleo

					if ((mensajeNucleo = recv(nucleofd, buf, sizeof(buf), 0)) <= 0) {
						// Error o conexión cerrada por el cliente
						if (mensajeNucleo == 0) {
							// Conexión cerrada
							printf("Select: El nucleo %d se ha desconectado\n", nucleofd);
						} else {
							perror("recv");
						}

					printf("El mensaje:  %s\n del nucleo ha sido enviado al Swap.\n",buf);
					send(swapfd,buf,256,0);

						FD_CLR(nucleofd, &master); // Eliminar del conjunto maestro
						bzero(&buf,256);            // Vaciar buffer

				// Gestionar conexion del cpu

						if ((cpufd = accept(listenercpu, (struct sockaddr*)&direccionCPU, &addrlen)) == -1)
											{
												perror("accept");
											} else {
												FD_SET(cpufd, &master);  // Añadir nucleo al conjunto maestro
												if (cpufd > fdmax) {    	// Actualizar el máximo
													fdmax = cpufd;
												}
												printf("Nueva conexion desde %s en "
														"socket %d\n", inet_ntoa(direccionCPU.sin_addr),cpufd);
											}


											// Gestionar datos del nucleo

											if ((mensajeCPU = recv(cpufd, buf, sizeof(buf), 0)) <= 0) {
												// Error o conexión cerrada por el cliente
												if (mensajeCPU == 0) {
													// Conexión cerrada
													printf("Select: La CPU %d se ha desconectado\n", cpufd);
												} else {
													perror("recv");
												}

											printf("El mensaje:  %s\n de la cpu ha sido enviado al Swap.\n",buf);
											send(swapfd,buf,256,0);

												FD_CLR(nucleofd, &master);  // Eliminar del conjunto maestro
												bzero(&buf,256);             // Vaciar buffer


		}
	}
	return EXIT_SUCCESS;
}


