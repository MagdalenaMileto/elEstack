/*
 * umc.h
 *
 *  Created on: 29/4/2016
 *      Author: utnso
 */

#ifndef SRC_UMC_H_
#define SRC_UMC_H_

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <signal.h>

#include <theDebuggers/socketLibrary.h>
#include <commons/config.h>

#include "globales.h"
#include "hilos/cpu.h"
#include "hilos/nucleo.h"
#include "consola/consola.h"
#include "consola/info.h"

void inicializar_semaforos();
void levantar_configuraciones();
void comunicarse_con_el_swap();
void esperar_al_nucleo();
void atender_conexiones();
void solicitar_bloque_memoria();
void atender_hilo_consola();
void mostrar_informacion_interna();
void borrar_archivos_existentes();
void crear_log();
void registrar_senial_cierre();

#endif /* SRC_UMC_H_ */
