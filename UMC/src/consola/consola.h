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

#include <commons/string.h>
#include <../commons/collections/list.h>

#include "../../src/globales.h"

void esperar_comando(void * parametros);
void cambiar_retardo(int retardo_numerico);
void vaciar_tlb();
char * string_drop_from_end(char * string, int cuantos);

#endif /* CONSOLA_CONSOLA_H_ */
