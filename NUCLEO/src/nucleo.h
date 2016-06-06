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


#include <parser/metadata_program.h>

#include <signal.h>
 #include "../../Sockets Maggie/src/socketLibrary.h"
 #include "../../Sockets Maggie/src/socketLibrary.c"
 #include "../../COMUNES/nsockets.h"
 //#include "../../COMUNES/nsockets.c"

 #include "../../COMUNES/estructurasControl.h"



#define PUERTO "9997"
#define BACKLOG 5			// Define cuantas conexiones vamos a mantener pendientes al mismo tiempo
#define PACKAGESIZE 1024	// Define cual va a ser el size maximo del paquete a enviar



typedef struct {
  char* PUERTO_PROG;
  char* PUERTO_CPU;
  int QUANTUM;
  int QUANTUM_SLEEP;
  char** SEM_IDS;
  char** SEM_INIT;
  char** IO_IDS;
  char** IO_SLEEP;
  char** SHARED_VARS;
  int STACK_SIZE;

//Mias
  char* IP_UMC;
  char* PUERTO_UMC;
  int SIZE_PAGINA;


  int* VALOR_SHARED_VARS;
  int* VALOR_SEM;
  int* VALOR_IO;

  long long*VALOR_IO_EXPIRED_TIME; 



}CONF_NUCLEO;



typedef struct __attribute__((packed))t_blocked{
	t_pcb pcb;
	int sem_offset;
	int IO_offset;
	int IO_time;
}t_blocked;




typedef struct{
  int socket_CPU;
  int socket_CONSOLA;
  t_pcb *pcb;
  long long tiempoBloqueado;
  char semaforoBloqueado;


}t_proceso;

long long *punteroConCero(char **ana1);
int *convertirConfigInt(char **ana1,char **ana2);

void get_config_nucleo (CONF_NUCLEO *config_nucleo);

void intHandler(int dummy);

void conectarUmc(void);

void *hilo_PLP(void *arg);
void *hilo_PCP(void *arg);


t_proceso* dameProceso(t_queue *cola, int sock );
void mandarAEjecutar(t_proceso *proceso,int sock);

void *hilo_mock(void *arg);


void *hilo_mock_consola(void *arg);
void *hilo_mock_cpu(void *arg);


t_pcb *desserializarPCB(t_pcb *serializado);
void destruirPCB(t_pcb *pcb);
t_pcb *serializarPCB(t_pcb *pcb);


void *hilo_CONEXION_CONSOLA(void *arg);
void *hilo_CONEXION_CPU(void *arg);


void *hilo_HANDLER_CONEXIONES_CPU(void *arg);
void *hilo_HANDLER_CONEXIONES_CONSOLA(void *arg);

long long current_timestamp(void);

#endif /* NUCLEO_H_ */
