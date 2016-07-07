/*
 * tlb.h
 *
 *  Created on: 2/6/2016
 *      Author: utnso
 */

#ifndef SRC_TLB_TLB_H_
#define SRC_TLB_TLB_H_

#include "../tabla_de_paginas/estructuras.h"
#include "../tabla_de_paginas/tabla_de_paginas.h"
#include "../globales.h"
#include "../commonsV2.h"

#include <string.h>
#include <stdbool.h>

t_entrada_tabla_de_paginas * buscar_tlb(int pid, int numero_pagina);
void agregar_entrada_tlb(t_entrada_tabla_de_paginas * entrada);
bool hay_entradas_tlb_disponibles();
void remplazo_lru(t_entrada_tabla_de_paginas * entrada);
int numero_operacion();
void eliminar_proceso_tlb(int pid);
bool tlb_habilitada();

#endif /* SRC_TLB_TLB_H_ */
