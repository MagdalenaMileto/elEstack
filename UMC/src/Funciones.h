/*
 * Funciones.h
 *
 *  Created on: 14 de may. de 2016
 *      Author: Micaela Paredes
 */

#ifndef FUNCIONES_H_
#define FUNCIONES_H_

void inicializarDireccionesLogicas(void);
void inicializarTablaDePaginas(void);
void inicializarPrograma(int idProg, int paginasRequeridas);
void almacenarBytes(int pagina, int offset, int tamanioBuffer, int buffer[tamanioBuffer]);
void solicitarBytes(int pagina, int offset, int cantidadBytes);
void cambioDeProcesoActivo(int idProg);
void finalizarPrograma(int idProg);
void almacenarBytes(int pagina, int offset, int tamanioBuffer, int buffer[tamanioBuffer]);
void solicitarBytes(int pagina, int offset, int cantidadBytes);
void cambioDeProcesoActivo(int idProg);
void rutinaNucleo(int cpufd);

void *rutinaCPU(void *argumentoSocket);
void *rutinaNucleo(void *argumentoSocket);

struct arg_sockets{
	int socket;
};

typedef struct{
	int offset[tamanioPagina];
}t_pagina;

typedef struct{
	int idPrograma;
	t_pagina pagina;
}regPrograma;

regPrograma direccionesLogicas[cantidadPaginas];

typedef int tipoPaginaMemoria;
// tipoPaginaMemoria memoria[MARCOS*MARCO_SIZE] = malloc(cantidadPaginas*tamanioPagina); // Memoria total
int tablaDePaginas[cantidadPaginas];


#endif /* FUNCIONES_H_ */
