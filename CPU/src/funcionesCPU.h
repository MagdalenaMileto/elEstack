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

typedef struct {
	 int QUANTUM;
	 int QUANTUM_SLEEP;
} t_datos_kernel;


typedef struct {
	t_puntero_instruccion start;
	t_size offset;
} t_instruccion;


t_puntero_instruccion buscar_etiqueta(const t_nombre_etiqueta objetivo, const char *etiquetas, const t_size etiquetas_size);
//hacer el diccionario de etiquetas
//t_puntero_instruccion seria la estrucutra con la las instrucciones..

typedef struct {
	t_puntero_instruccion instruccion_inicio; //El numero de la primera instruccion (Begin)
	t_size instrucciones_size; // Cantidad de instrucciones
	t_intructions* instrucciones_serializado; // Instrucciones del programa
	t_size etiquetas_size; // Tamaño del mapa serializado de etiquetas
	char* etiquetas; // La serializacion de las etiquetas
	int	cantidad_de_funciones;
	int	cantidad_de_etiquetas;
} t_data_programa;





void get_config_cpu (CONF_CPU *config_cpu);
int conectarConUmc();
int conectarConNucleo();
t_paquete* recibirPCB(int nucleo);
t_paquete* crearPaquete(void* data, int codigo, int size);
t_paquete* enviarInstruccionAMemoria(int umc, int* indice, int32_t offset, uint32_t tamanio);
void ejecutarInstruccion(t_pcb* pcb, int umc, int QUANTUM_SLEEP);


#endif /* FUNCIONESCPU_H_ */
