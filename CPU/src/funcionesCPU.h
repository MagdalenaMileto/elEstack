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
  int PUERTO_NUCLEO;
  char* IP_NUCLEO;
  int PUERTO_UMC;
  char* IP_UMC;
  int STACK_SIZE;
  int SIZE_PAGINA;
}CONF_CPU;


void get_config_cpu (CONF_CPU *config_cpu);
int conectarConUmc();
int conectarConNucleo();
t_paquete* recibirPCB(int nucleo);
t_datos_kernel* desserializarDatosKernel(char* paquete_kernel);
char* depurarSentencia(char* sentencia);
t_direccion*  crearEstructuraParaUMC (t_pcb* pcb, t_datos_kernel* info_kernel);


#endif /* FUNCIONESCPU_H_ */
