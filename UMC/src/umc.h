/*
 * umc.h
 *
 *  Created on: 29/4/2016
 *      Author: utnso
 */

#ifndef SRC_UMC_H_
#define SRC_UMC_H_
#include <theDebuggers/socketLibrary.h>
#include "globales.h"
#include "hilos/cpu.h"
#include "hilos/nucleo.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>

#include <commons/config.h>


void inicializar_semaforos();
void levantar_configuraciones();
void comunicarse_con_el_swap();
void esperar_al_nucleo();
void atender_conexiones();
void solicitar_bloque_memoria();

#endif /* SRC_UMC_H_ */
