/*
 * cpu.h
 *
 *  Created on: 29/4/2016
 *      Author: utnso
 */

#ifndef SRC_HILOS_CPU_H_
#define SRC_HILOS_CPU_H_

#include <stdio.h>
#include <unistd.h>

#include "../pedidos/leer.h"
#include "../globales.h"

#include <theDebuggers/socketLibrary.h>

#define LEER 1
#define ESCRIBIR 2
#define CAMBIAR_PROCESO 3
#define CERRAR 4

void atender_cpu(void * parametro_hilo);

#endif /* SRC_HILOS_CPU_H_ */
