/*
 * tabla_de_paginas.h
 *
 *  Created on: 17/5/2016
 *      Author: utnso
 */

#ifndef SRC_TABLA_DE_PAGINAS_TABLA_DE_PAGINAS_H_
#define SRC_TABLA_DE_PAGINAS_TABLA_DE_PAGINAS_H_

#include <stdlib.h>

#include  "../globales.h"
#include "../commonsV2.h"
#include "estructuras.h"

t_entrada_tabla_de_paginas * crear_nueva_entrada(int pid, int pagina);
void eliminar_proceso_tabla_de_paginas(int pid);


#endif /* SRC_TABLA_DE_PAGINAS_TABLA_DE_PAGINAS_H_ */
