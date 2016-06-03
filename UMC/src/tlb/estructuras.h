/*
 * estructuras.h
 *
 *  Created on: 2/6/2016
 *      Author: utnso
 */

#ifndef SRC_TLB_ESTRUCTURAS_H_
#define SRC_TLB_ESTRUCTURAS_H_

#include <commons/collections/list.h>

typedef struct {
	int pid;
	int pagina;
	int marco;
	bool uso;
	bool modificado;
	bool presencia;
	int ultima_vez_usado;
} t_entrada_tlb;

#endif /* SRC_TLB_ESTRUCTURAS_H_ */
