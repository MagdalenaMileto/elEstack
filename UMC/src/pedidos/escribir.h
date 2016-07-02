/*
 * escribir.h
 *
 *  Created on: 17/5/2016
 *      Author: utnso
 */

#ifndef SRC_PEDIDOS_ESCRIBIR_H_
#define SRC_PEDIDOS_ESCRIBIR_H_

#include "../../src/tlb/tlb.h"
#include "../../src/tabla_de_paginas/tabla_de_paginas.h"
#include "../../src/tabla_de_paginas/marcos.h"

#include "../globales.h"

void escribir_una_pagina(int pid, int numero_pagina, int offset, int tamanio,
		void * buffer);

#endif /* SRC_PEDIDOS_ESCRIBIR_H_ */
