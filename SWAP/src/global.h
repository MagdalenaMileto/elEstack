/*
 * global.h
 *
 *  Created on: 27/6/2016
 *      Author: utnso
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_
#include <commons/collections/list.h>
#include <stdbool.h>


t_list * paginasSWAP;
void *discoParaleloNoVirtualMappeado;
char* NOMBRE_SWAP;
char* PUERTO_SWAP;
int CANTIDAD_PAGINAS;
int TAMANIO_PAGINA;
int RETARDO_COMPACTACION;
int RETARDO_ACCESO;

long int tamanio_archivo;


typedef struct {
	int pid;
	bool libre;
	int posicion;
} t_controlPaginas;

#endif /* GLOBAL_H_ */
