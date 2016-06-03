/*
 * inicializar.h
 *
 *  Created on: 13/5/2016
 *      Author: utnso
 */

#ifndef SRC_PEDIDOS_INICIALIZAR_H_
#define SRC_PEDIDOS_INICIALIZAR_H_

#include <stdbool.h>

//TODO: Preguntarle a fede
//#include "interfaces/swap.h"
#include "../globales.h"
#include "../tabla_de_paginas/estructuras.h"
#include "../tabla_de_paginas/tabla_de_paginas.h"

bool puede_iniciar_proceso(int pid, int cantidad_paginas, char * codigo);

void inicializar_programa(int pid, int paginas_requeridas);

#endif /* SRC_PEDIDOS_INICIALIZAR_H_ */
