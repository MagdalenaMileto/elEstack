/*
 * Estructuras.h
 *
 *  Created on: May 15, 2016
 *      Author: utnso
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

struct arg_sockets{
	int socket;
};

typedef struct{
	char offset[MARCO_SIZE];
}t_pagina;

typedef struct{
	int idPrograma;
	t_pagina pagina;
}regPrograma;

#endif /* ESTRUCTURAS_H_ */
