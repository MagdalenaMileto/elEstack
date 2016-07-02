/*
 * leer.h
 *
 *  Created on: 13/5/2016
 *      Author: utnso
 */

#ifndef SRC_PEDIDOS_LEER_H_
#define SRC_PEDIDOS_LEER_H_

#include <stdlib.h>
#include <string.h>

#include "../commonsV2.h"
#include "../globales.h"
#include "../tabla_de_paginas/marcos.h"

#include "../pedidos/interfaces/swap.h"

void * leer_una_pagina(int pid,int numero_pagina, int offset, int tamanio);
bool no_tiene_ni_hay_marcos(int proceso_actual);

#endif /* SRC_PEDIDOS_LEER_H_ */
