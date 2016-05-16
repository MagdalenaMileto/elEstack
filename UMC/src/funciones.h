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
void leerArchivoDeConfiguracion(void);
char IP_SWAP[16];
int PUERTO_SWAP, PUERTO_NUCLEO, PUERTO_CPU;
int MARCOS;
int MARCO_SIZE;

void *rutinaCPU(void *argumentoSocket);
void *rutinaNucleo(void *argumentoSocket);


#endif /* FUNCIONES_H_ */
