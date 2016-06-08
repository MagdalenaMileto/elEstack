/*
 * socketLibrary.c
 *
 *  Created on: 29/4/2016
 *      Author: utnso
 */

#include "socketLibrary.h"

/**  @NAME: _configurar_addrinfo
 *	 @DESC: Retorna un puntero a una addrInfo totalmente lista para usar,
 *	 dado una IP y un Host cualesquiera.
 *	 SOLO para uso interno.
 */

struct addrinfo* _configurar_addrinfo(char *IP, char* Port) {
	struct addrinfo hints;
	struct addrinfo* serverInfo;
	int16_t error;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if (!strcmp(IP, "localhost")) {
		hints.ai_flags = AI_PASSIVE;
		error = getaddrinfo(NULL, Port, &hints, &serverInfo);
	} else
		error = getaddrinfo(IP, Port, &hints, &serverInfo);
	if (error) {
		error_show("Problema con el getaddrinfo()!: %s", gai_strerror(error));
		exit(EXIT_FAILURE);
	}
	return serverInfo;
}

un_socket conectar_a(char *IP, char* Port) {
	struct addrinfo* serverInfo = _configurar_addrinfo(IP, Port);
	if (serverInfo == NULL) {
		exit(EXIT_FAILURE);
	}
	int serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);
	if (serverSocket == -1) {
		error_show("\n No se pudo conectar",
		errno);
		exit(EXIT_FAILURE);
	}
	if (connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen)
			== -1) {
		puts("\n");
		error_show(
				"No se pudo conectar con el proceso que hace de servidor, error: %d",
				errno);
		close(serverSocket);
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(serverInfo);
	return serverSocket;
}

un_socket socket_escucha(char* IP, char* Port) {

	struct addrinfo* serverInfo = _configurar_addrinfo(IP, Port);

	if (serverInfo == NULL) {
		exit(EXIT_FAILURE);
	}

	int socketEscucha;

	socketEscucha = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);

	int enable = 1;setsockopt(socketEscucha, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));//forzar la reutilizacion del socket 
	bind(socketEscucha, serverInfo->ai_addr, serverInfo->ai_addrlen);

	freeaddrinfo(serverInfo);

	return socketEscucha;
}

void enviar(un_socket socket_para_enviar, int codigo_operacion, int tamanio,
		void * data) {

	int tamanio_paquete = 2 * sizeof(int) + tamanio;
	void * buffer = malloc(tamanio_paquete);

	memcpy(buffer, &codigo_operacion, sizeof(int));
	memcpy(buffer + sizeof(int), &tamanio, sizeof(int));
	memcpy(buffer + 2 * sizeof(int), data, tamanio);

	send(socket_para_enviar, buffer, tamanio_paquete, MSG_WAITALL);

	free(buffer);

}

t_paquete* recibir(un_socket socket_para_recibir) {

	t_paquete * paquete_recibido = malloc(sizeof(t_paquete));

	recv(socket_para_recibir, &paquete_recibido->codigo_operacion, sizeof(int),
	MSG_WAITALL);
	recv(socket_para_recibir, &paquete_recibido->tamanio, sizeof(int),
	MSG_WAITALL);

	void * informacion_recibida = malloc(paquete_recibido->tamanio);

	recv(socket_para_recibir, informacion_recibida, paquete_recibido->tamanio,
	MSG_WAITALL);

	paquete_recibido->data = informacion_recibida;

	return paquete_recibido;
}

un_socket aceptar_conexion(un_socket socket_escuchador) {

	struct sockaddr_storage remoteaddr;

	socklen_t addrlen;

	addrlen = sizeof remoteaddr;

	un_socket nuevo_socket = accept(socket_escuchador,
			(struct sockaddr *) &remoteaddr, &addrlen);

	return nuevo_socket;
}

void liberar_paquete(t_paquete * paquete) {
	free(paquete->data);
	free(paquete);
}

bool realizar_handshake(un_socket socket_del_servidor) {

	char * mensaje = malloc(18);
	mensaje = "Inicio autenticacion";

	enviar(socket_del_servidor, 1, 21, mensaje);

	t_paquete * resultado_del_handhsake = recibir(socket_del_servidor);

	bool resultado = string_equals_ignore_case(
			(char *) resultado_del_handhsake->data, "Autenticado");

	liberar_paquete(resultado_del_handhsake);

	return resultado;

}

bool esperar_handshake(un_socket socket_del_cliente) {

	t_paquete * inicio_del_handhsake = recibir(socket_del_cliente);

	bool resultado = string_equals_ignore_case(
			(char *) inicio_del_handhsake->data, "Inicio autenticacion");

	liberar_paquete(inicio_del_handhsake);

	if (resultado) {

		char * respuesta = malloc(12);
		respuesta = "Autenticado";
		enviar(socket_del_cliente, 1, 12, respuesta);

	} else {

		char * respuesta = malloc(6);
		respuesta = "Error";
		enviar(socket_del_cliente, 1, 6, respuesta);

	}

	return resultado;
}

