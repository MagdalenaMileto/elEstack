/*
 * estructuras.h
 *
 *  Created on: 19/5/2016
 *      Author: utnso
 */

#ifndef SRC_TABLA_DE_PAGINAS_ESTRUCTURAS_H_
#define SRC_TABLA_DE_PAGINAS_ESTRUCTURAS_H_

//#include <stdbool.h>

#include <commons/collections/list.h>

typedef struct {
	int pid;
	int pagina;
	int marco;
	bool uso;
	bool modificado;
	bool presencia;
	int ultima_vez_usado;
} t_entrada_tabla_de_paginas;

#endif /* SRC_TABLA_DE_PAGINAS_ESTRUCTURAS_H_ */
