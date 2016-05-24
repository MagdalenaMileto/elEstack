/*
 * socket.c
 *
 *  Created on: 24/5/2016
 *      Author: utnso
 */

#include "socket.h"


int iniciarServidor(int puerto) {

	int sock_lst;  // Escuchar sobre sock_lst, nuevas conexiones sobre new_lst
	struct sockaddr_in my_addr;    // información sobre mi dirección
	my_addr.sin_family = AF_INET;         // Ordenación de bytes de máquina
	my_addr.sin_port = htons(puerto);     // short, Ordenación de bytes de la red
	my_addr.sin_addr.s_addr = INADDR_ANY; // rellenar con mi dirección IP
	memset(&(my_addr.sin_zero), '\0', 8); // poner a cero el resto de la estructura
	int yes=1;

	if ((sock_lst = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return -1;
	}

	if (setsockopt(sock_lst,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		perror("setsockopt");
		return -1;
	}

	if (bind(sock_lst, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		return -1;
	}

	if (listen(sock_lst, 10) == -1) {
		perror("listen");
		return -1;
	}

	return sock_lst;
}

int establecerConexion(int sock_lst) {
	int new_lst;
	struct sockaddr_in umcAddress; // información sobre la dirección del cliente
	unsigned int sin_size = sizeof(struct sockaddr_in);
	if ((new_lst = accept(sock_lst, (struct sockaddr *)&umcAddress, &sin_size)) == -1)
	{
		perror("accept");
	}
	else {
        printf("SWAP: Nueva conexion desde %s en" "socket %d\n",inet_ntoa(umcAddress.sin_addr), new_lst);
        return -1;
	}
	return new_lst;
}

int recibirMensaje(int new_lst, MPS_MSG *mensaje) {
	int retorno;
	t_header cabecera;
	mensaje->tam_payload = 0;
	int largoHeader = sizeof cabecera.id_payload + sizeof cabecera.tam_payload;

	// Recibo Primero el Header

	retorno = recibirInfo(new_lst, (unsigned char *) &cabecera, largoHeader);
	if (retorno <= 0) {
		//cerrarConexionPorFalloDeRecepcion(new_lst);
		return -1;
	}
	// Desarmo la cabecera
	mensaje->id_payload = cabecera.id_payload;
	mensaje->tam_payload = cabecera.tam_payload;

	// Si es necesario recibo Primero el PayLoad.
	if (mensaje->tam_payload != 0) {
		mensaje->Payload = malloc(mensaje->tam_payload + 1);
		memset(mensaje->Payload, '\0', mensaje->tam_payload + 1);

		retorno = recibirInfo(new_lst, mensaje->Payload, (mensaje->tam_payload));
	} else {
		mensaje->Payload = NULL;
	}
	return retorno;
}

int recibirInfo(int new_lst, buffer_t Buffer, int CantidadARecibir) {
	long Recibido = 0;
	long TotalRecibido = 0;
	while (TotalRecibido < CantidadARecibir) {
		Recibido = recv(new_lst, Buffer + TotalRecibido,CantidadARecibir - TotalRecibido, 0);

		switch (Recibido) {
		case 0:
			return TotalRecibido;
			break;

		case -1:
			return -1;
			break;

		default:
			TotalRecibido += Recibido;
			break;
		}
	}
	return TotalRecibido;
}


