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

#include "../pedidos/interfaces/swap.h"

void * leer_una_pagina(int numero_pagina, int offset, int tamanio);
void * buscar_pagina_tabla_de_paginas(int numero_pagina);

#endif /* SRC_PEDIDOS_LEER_H_ */
