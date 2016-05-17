/*
 * nsockets.h
 *
 *  Created on: 26/4/2016
 *      Author: utnso
 */

#ifndef NSOCKETS_H_
#define NSOCKETS_H_

typedef struct{
	int32_t id;
	int32_t size;
	void* data;
}t_header;

int32_t enviar_paquete(int32_t enlace,t_header header_a_enviar);
int32_t recibir_paquete(int32_t enlace, t_header* header_a_recibir);
int32_t escribir_socket (int32_t nuevo_socket, char *datos, size_t longitud);
int32_t leer_socket (int32_t nuevo_socket, char *buffer, size_t size);
int32_t abrir_servidor (int puerto, int max_conexiones);//
int32_t aceptar_cliente (int descriptor);
int32_t cerrar_socket (int descriptor);
int32_t liberar_puerto (int descriptor);
int32_t set_nonblocking(int descriptor);
signed int cliente(char *ip_server, int puerto);
int enviar_id(int conexion,int id);
int32_t servidor (int puerto);

#endif /* NSOCKETS_H_ */
