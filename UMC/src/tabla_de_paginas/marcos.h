/*
 * marcos.h
 *
 *  Created on: 5/6/2016
 *      Author: utnso
 */

#ifndef TABLA_DE_PAGINAS_MARCOS_H_
#define TABLA_DE_PAGINAS_MARCOS_H_

#include "../globales.h"
#include "../commonsV2.h"
#include "../pedidos/interfaces/swap.h"
#include "estructuras.h"

#include <stdbool.h>
#include <stdlib.h>

void marco_nuevo(t_entrada_tabla_de_paginas * entrada_sin_marco);
bool tiene_cantidad_maxima_marcos_asignados(int proceso_actual);
void inicializar_marcos();
void algoritmo_remplazo(t_entrada_tabla_de_paginas * entrada_sin_marco, int pid);
t_list * lista_circular_clock(t_list * lista, int pid);
void avanzar_victima(t_list * lista_clock,
		t_entrada_tabla_de_paginas * entrada_con_marco_nuevo,
		t_entrada_tabla_de_paginas * victima);
bool de_una_entrada(t_list * lista);
void escribir_marco(int marco, int offset, int tamanio, void * contenido);
bool hay_marcos_disponibles();

#endif /* TABLA_DE_PAGINAS_MARCOS_H_ */
