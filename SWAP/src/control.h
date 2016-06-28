/*
 * control.h
 *
 *  Created on: 27/6/2016
 *      Author: utnso
 */

#ifndef CONTROL_H_
#define CONTROL_H_
#include "global.h"
#include <stdbool.h>
#include <string.h>

#include <stdlib.h>

void finalizar_proceso(int pid);
void escribir(int pid, int pagina, void * contenido);
void * leer(int pid, int pagina);
void escribirPagina(int posicion, void * contenido);
void * leerPagina(int posicion);
void inicializarProceso(int pid, int cantidad_paginas, void * codigo);
bool sePuedeInicializar(int cantidad_paginas);
t_list * paginas_para_asignar(int cantidad_paginas);
void inicializarControlPaginas(int cantidad_paginas);
void compactacion();

#endif /* CONTROL_H_ */
