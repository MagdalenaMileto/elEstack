/*
 * socket.h
 *
 *  Created on: 24/5/2016
 *      Author: utnso
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<unistd.h>

typedef unsigned char* buffer_t;


typedef struct _MPS_MSG
{
 int8_t id_payload;
 int16_t tam_payload;
 void *Payload;
} __attribute__((__packed__)) MPS_MSG;


typedef struct _t_header
{
   int8_t id_payload;
   int16_t tam_payload;
} __attribute__((__packed__)) t_header;


int iniciarServidor();
int establecerConexion(int sock_lst);
int recibirMensaje(int Socket, MPS_MSG *mensaje);



#endif /* SOCKET_H_ */
