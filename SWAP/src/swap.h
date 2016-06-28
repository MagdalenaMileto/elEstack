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
#include <theDebuggers/socketLibrary.h>
#include "global.h"
#include "control.h"
#include <unistd.h>

#define INICIALIZAR 1
#define LEER 3
#define ESCRIBIR 4
#define FINALIZAR 2

#define EXITO 1
#define NOHAYLUGAR -1

t_config* configuracion;

//typedef struct __attribute__((packed)){
//	int ocupada;
//	int idProcesoQueLoOcupa;
//}pagina;
//
//typedef struct __attribute__((packed)){
//	int cantPagsUsando;
//	int idProceso;
//}proceso;

int	abrirConfiguracion();
int	crearArchivo();
int mapearArchivo();
//int asignarEspacio(int cantPagsAAsignar, int proceso, int inicio);
//int ultimaPagLibre();
//int compactacion();
//int hayLugarParaNuevoProceso(int cantPagsNecesita);
//int primerPaginaOcupadaLuegoDeUnaLibreDada(int nroPag);
//int primerPagLibreAPartirDeUnaDada(int numeroPag);
//int reservarProceso(int pidProceso, int cantPags , int pagAPartir);
//void nuevoProc(int pid);
//int primerPaginaOcupadaLuegoDeUnaLibre();
//int leerPagina( int nroPag, long int*inicio, long int*fin);
//int hayPaginasOcupadasLuegoDeLaUltimaLibre();
//int updatearArchivoDisco();
//int liberarProceso(int idProc);
//int getPrimerPagProc(int idProceso);
//int getPosicionDelProceso(int idProc);
//int escribirPaginaProceso(int idProceso, int nroPag, void*data);
//int escribirPagina(int nroPag, void*dataPagina);
//int leerPaginaProceso(int idProceso, int nroPag, void* paginaALeer);
//int inicializarEstructuraPaginas();
//void llenarprocPags();
void *hilo_mock(void *arg);
void check (int test, const char * message, ...);


#endif /* SWAP_H_ */
