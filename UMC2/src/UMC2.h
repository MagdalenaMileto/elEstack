/*
 * UMC2.h
 *
 *  Created on: 29/5/2016
 *      Author: utnso
 */

#ifndef UMC2_H_
#define UMC2_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <commons/config.h>

#include "../../COMUNES/nsockets.c"

#include <signal.h>


//Archivo Configuracion
t_config* configuracion;
int PUERTO, PUERTO_SWAP, MARCOS, MARCO_SIZE, MARCO_X_PROC, ENTRADAS_TLB, RETARDO;
char *IP_SWAP;
char *ALGORITMO;

typedef struct __attribute__((packed)){
	int pid;
	int pedido;
	int pagina;
	char texto[1024];
	int flagProc;
} paquete;

int abrirConfiguracion();
int conectarConSwap();
int inicializarPrograma (int idProg, int cantPag, char codigo);
void *hilo_Conexion(void *arg);
void *hilo_mock(void *arg);




#endif /* UMC2_H_ */
