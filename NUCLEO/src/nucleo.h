/*
 * nucleo.h
 *
 *  Created on: 28/4/2016
 *      Author: utnso
 */

#ifndef NUCLEO_H_
#define NUCLEO_H_
#include <math.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>


#include <commons/collections/node.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
 #include <commons/config.h>


#include <signal.h>


 #include "../../COMUNES/nsockets.h"

 #include "../../COMUNES/estructurasControl.h"



#define PUERTO "9997"
#define BACKLOG 5			// Define cuantas conexiones vamos a mantener pendientes al mismo tiempo
#define PACKAGESIZE 1024	// Define cual va a ser el size maximo del paquete a enviar


typedef struct{
  int socket_CPU;
  int socket_PROGRAMA;
  t_pcb pcb;
}t_proceso;



void intHandler(int dummy);

void conectarUmc(void);

void *hilo_PLP(void *arg);
void *hilo_PCP(void *arg);


t_proceso* dameProceso(t_queue *cola, int sock );
void mandarAEjecutar(t_proceso *proceso,int sock);

void *hilo_mock(void *arg);


void *hilo_mock_consola(void *arg);
void *hilo_mock_cpu(void *arg);





void *hilo_CONEXION_CONSOLA(void *arg);
void *hilo_CONEXION_CPU(void *arg);


void *hilo_HANDLER_CONEXIONES_CPU(void *arg);
void *hilo_HANDLER_CONEXIONES_CONSOLA(void *arg);



#endif /* NUCLEO_H_ */
