/*
 * marcos.h
 *
 *  Created on: 5/6/2016
 *      Author: utnso
 */

#ifndef TABLA_DE_PAGINAS_MARCOS_H_
#define TABLA_DE_PAGINAS_MARCOS_H_

#include "../globales.h"
#include "estructuras.h"

#include <stdbool.h>
#include <stdlib.h>

int marco_nuevo();
bool tiene_cantidad_maxima_marcos_asignados(int proceso_actual);
void inicializar_marcos();


#endif /* TABLA_DE_PAGINAS_MARCOS_H_ */
