/*
 * swap.h
 *
 *  Created on: 20/5/2016
 *      Author: utnso
 */

#ifndef SWAP_H_
#define SWAP_H_

#include<commons/log.h>
#include<commons/config.h>
#include<commons/txt.h>
#include<sys/mman.h>
#include "socket.h"


#define Puerto 1206

char* Nombre_Swap;
int Puerto_Swap;
int Cantidad_Paginas;
int Tamanio_Pagina;
int Retardo_Compactacion;

t_config* configuracion;
long int tamanio_archivo;

int	abrirConfiguracion();
int	crearArchivo();
int mapearArchivo();
int asignarEspacio(int cantPagsAAsignar, int contadorP, int proceso, int inicio);
int ultimaPagLibre();
int compactacion();
int hayLugarParaNuevoProceso(int cantPagsNecesita);
int primerPaginaOcupadaLuegoDeUnaLibreDada(int nroPag);
int primerPagLibreAPartirDeUnaDada(int numeroPag);
int reservarProceso(int pidProceso, int cantPags , int pagAPartir);
void nuevoProc(int pid);
int primerPaginaOcupadaLuegoDeUnaLibre();
int leerPagina( int nroPag, long int*inicio, long int*fin);
int hayPaginasOcupadasLuegoDeLaUltimaLibre();
int updatearArchivoDisco();
int liberarProceso(int idProc);
int getPrimerPagProc(int idProceso);
int getPosicionDelProceso(int idProc);

typedef struct __attribute__((packed)){
	int pid;
	int nroPag;
	int posicionSwap;
	char texto[1024];
	int flagProc; //
} paqueteUMC;

typedef struct __attribute__((packed)){
	int ocupada;
	int idProcesoQueLoOcupa;
}pagina;

typedef struct __attribute__((packed)){
	int cantPagsUsando;
	int idProceso;
}proceso;


#endif /* SWAP_H_ */
