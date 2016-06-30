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
//#include "global.h"
//#include "control.h"
#include <unistd.h>

#define INICIALIZAR 1
#define LEER 3
#define ESCRIBIR 4
#define FINALIZAR 2

#define EXITO 1
#define NOHAYLUGAR -1

t_config* configuracion;

////
void *discoParaleloNoVirtualMappeado;
char* NOMBRE_SWAP;
char* PUERTO_SWAP;
int CANTIDAD_PAGINAS;
int TAMANIO_PAGINA;
int RETARDO_COMPACTACION;
int RETARDO_ACCESO;

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
void crearArchivo();
void mapearArchivo();
int asignarEspacio(int cantPagsAAsignar, int proceso, int inicio);
int ultimaPagLibre();
int compactacion();
int hayLugarParaNuevoProceso(int cantPagsNecesita);
int primerPaginaOcupadaLuegoDeUnaLibreDada(int nroPag);
int primerPagLibreAPartirDeUnaDada(int numeroPag);
int reservarProceso(int pidProceso, int cantPags , int pagAPartir);
void nuevoProc(int pid);
int primerPaginaOcupadaLuegoDeUnaLibre();
long int obtenerlugarDeInicioDeLaPagina(int nroPag);
int hayPaginasOcupadasLuegoDeLaUltimaLibre();
int updatearArchivoDisco();
void liberarProceso(int idProc);
int getPrimerPagProc(int idProceso);
void escribirPaginaProceso(int idProceso, int nroPag, void*data);
void escribirPagina(int nroPag, void*dataPagina);
void leerPaginaProceso(int idProceso, int nroPag, void* paginaALeer);
void inicializarEstructuraPaginas();
void llenarprocPags();
void inicializarProceso(int pid, int pagina, char*codigo);
void *hilo_mock(void *arg);
void check (int test, const char * message, ...);


#endif /* SWAP_H_ */
