/*
 * funcionesCPU.h
 *
 *  Created on: 31/5/2016
 *      Author: utnso
 */

#ifndef FUNCIONESCPU_H_
#define FUNCIONESCPU_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <parser/parser.h>
#include <parser/sintax.h>
#include <parser/metadata_program.h>
#include "../../COMUNES/nsockets.h"
#include "../../COMUNES/estructurasControl.h"
#include "../../COMUNES/estructurasControl.c"
#include "../../COMUNES/handshake.h"
#include "../../COMUNES/nsockets.c"
#include "primitivas.h"
#include "primitivas.h"
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <theDebuggers/socketLibrary.h>

typedef struct {
	 int QUANTUM;
	 int QUANTUM_SLEEP;
	 int TAMPAG;
} t_datos_kernel;


typedef struct {
  int PUERTO_NUCLEO;
  char* IP_NUCLEO;
  int PUERTO_UMC;
  char* IP_UMC;
  int STACK_SIZE;
  int SIZE_PAGINA;
}CONF_CPU;

int umc, nucleo;
t_pcb* pcb;
int quantum;
int tamanioPag;
int quantum_sleep;


int conectarConUmc();
int conectarConNucleo();
t_direccion*  crearEstructuraParaUMC (t_pcb* pcb, int tamPag);
void levantar_configuraciones();
char* depurarSentencia(char* sentencia);

/* Emi para tener un machete de los codigos de operacion y no confundirnos:
 *
 * 303->recibo pcb serializado
 * 352->recibo variable
 * 342->recibo valor del semaforo
 *
 * 304->envio pcb serializado por fin de quantum
 * 320->envio finalizacion del programa
 * 340->envio pcb bloqueado
 * 341->envio pido un semaforo
 * 342->envio liberar semaforo
 * 350->envio escribe una variable
 * 351->envio pide la variable
 * 360->envio impromir un valor numerico
 * 361->envio imprimir texto
 */

#endif /* FUNCIONESCPU_H_ */
