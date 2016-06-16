/*
 * consola.h
 *
 *  Created on: 15/6/2016
 *      Author: utnso
 */

#ifndef CONSOLA_CONSOLA_H_
#define CONSOLA_CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include <commons/string.h>
#include <../commons/collections/list.h>

#include "../../src/globales.h"
#include "../../src/tabla_de_paginas/estructuras.h"

void esperar_comando(void * parametros);
void cambiar_retardo(int retardo_numerico);
void * generar_estado_actual();
void vaciar_tlb();
void marcar_las_paginas_del_proceso_como_modificadas(int proceso);
char * string_drop_from_end(char * string, int cuantos);


bool isNumber(char * palabra);

#endif /* CONSOLA_CONSOLA_H_ */
