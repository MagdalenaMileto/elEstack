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
#include <string.h>

#include <commons/string.h>
#include <../commons/collections/list.h>

#include "../../src/globales.h"
#include "../../src/tabla_de_paginas/estructuras.h"

void esperar_comando(void * parametros);
void cambiar_retardo(int retardo_numerico);

void flush_tlb();
void flush_memory(int proceso);

void dump_proceso(int proceso);
void dump_total();
void dump_entrada(t_entrada_tabla_de_paginas * entrada);




void dump_proceso_iterate(void * elemento);
bool isNumber(char * palabra);
void escribir_a_dump(char * string);

#endif /* CONSOLA_CONSOLA_H_ */
