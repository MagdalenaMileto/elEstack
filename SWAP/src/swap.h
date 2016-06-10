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
#include <theDebuggers/socketLibrary.h>


char* NOMBRE_SWAP;
char* PUERTO_SWAP;
int CANTIDAD_PAGINAS;
int TAMANIO_PAGINA;
int RETARDO_COMPACTACION;
int RETARDO_ACCESO;

t_config* configuracion;
long int tamanio_archivo;

typedef struct __attribute__((packed)){
	int ocupada;
	int idProcesoQueLoOcupa;
}pagina;

typedef struct __attribute__((packed)){
	int cantPagsUsando;
	int idProceso;
}proceso;

int	abrirConfiguracion();
int	crearArchivo();
int mapearArchivo();
int asignarEspacio(int cantPagsAAsignar, int proceso, int inicio);
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
int escribirPaginaProceso(int idProceso, int nroPag, char*data);
int escribirPagina(int nroPag, char*dataPagina);
int leerPaginaProceso(int idProceso, int nroPag, char* paginaALeer);
int inicializarEstructuraPaginas();
void llenarprocPags();
void *hilo_mock(void *arg);


#endif /* SWAP_H_ */
