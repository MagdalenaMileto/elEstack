/*
 * control.c
 *
 *  Created on: 27/6/2016
 *      Author: utnso
 */

#include "control.h"

void finalizar_proceso(int pid) {
	bool esElPid(void * elemento) {
		t_controlPaginas* pagina = (t_controlPaginas*) elemento;
		return pagina->pid == pid;
	}

	t_list * paginas_a_borrar = list_filter(paginasSWAP, esElPid);
	void vaciar(void * elemento) {
		t_controlPaginas* pagina = (t_controlPaginas*) elemento;
		pagina->libre = true;
	}

	list_iterate(paginas_a_borrar, vaciar);
}

void escribir(int pid, int pagina, void * contenido) {
	bool esElPid(void * elemento) {
		t_controlPaginas* pagina = (t_controlPaginas*) elemento;
		return pagina->pid == pid;
	}

	t_list * paginasProceso = list_filter(paginasSWAP, esElPid);
	t_controlPaginas* paginaElegida = list_get(paginasProceso, pagina);

	escribirPagina(paginaElegida->posicion, contenido);

}
void * leer(int pid, int pagina) {
	bool esElPid(void * elemento) {
		t_controlPaginas* pagina = (t_controlPaginas*) elemento;
		return pagina->pid == pid;
	}
	t_list * paginasProceso = list_filter(paginasSWAP, esElPid);
	t_controlPaginas* paginaElegida = list_get(paginasProceso, pagina);

	return leerPagina(paginaElegida->posicion);
}

void inicializarControlPaginas(int cantidad_paginas) {
	paginasSWAP = list_create();
	int i;
	for (i = 0; i < cantidad_paginas; i++) {
		t_controlPaginas * pagina = malloc(sizeof(t_controlPaginas));
		pagina->libre = true;
		pagina->pid = -1;
		pagina->posicion = i;

		list_add(paginasSWAP, pagina);
	}
}

void escribirPagina(int posicion, void * contenido) {
	int desplazamiento = posicion * TAMANIO_PAGINA;
	memcpy(discoParaleloNoVirtualMappeado + desplazamiento, contenido,
			TAMANIO_PAGINA);
}
void * leerPagina(int posicion) {
	int desplazamiento = posicion * TAMANIO_PAGINA;
	return discoParaleloNoVirtualMappeado + desplazamiento;
}

void inicializarProceso(int pid, int cantidad_paginas, void * codigo) {
	t_list * huecos = paginas_para_asignar(cantidad_paginas);
	void * buffer = codigo;

	void asignar(void * elemento) {
		t_controlPaginas * pagina = (t_controlPaginas *) elemento;

		pagina->pid = pid;
		pagina->libre = false;

		escribirPagina(pagina->posicion, buffer);
		buffer += TAMANIO_PAGINA;
	}

	list_iterate(huecos, asignar);

}
bool sePuedeInicializar(int cantidad_paginas) {
	return !list_is_empty(paginas_para_asignar(cantidad_paginas));
}

t_list * paginas_para_asignar(int cantidad_paginas) {
	t_list * espacios_vacios = list_create();
	t_list * lista_auxiliar = NULL;

	void agrupar_espacios(void * elemento) {
		t_controlPaginas * pagina = (t_controlPaginas *) elemento;
		if (pagina->libre) {
			if (lista_auxiliar == NULL) {
				lista_auxiliar = list_create();
			}
			list_add(lista_auxiliar, pagina);
		} else {
			if (lista_auxiliar != NULL) {

				list_add(espacios_vacios, lista_auxiliar);
				lista_auxiliar = NULL;
			}
		}
	}

	list_iterate(paginasSWAP, agrupar_espacios);
	if (lista_auxiliar != NULL) {
		list_add(espacios_vacios, lista_auxiliar);
	}
	bool tiene_espacio(void * elemento) {
		t_list * lista = (t_list *) elemento;
		return list_size(lista) >= cantidad_paginas;
	}

	t_list * lista_con_espacio = list_find(espacios_vacios, tiene_espacio);

	if (lista_con_espacio == NULL) {
		return list_create();
	}

	return list_take(lista_con_espacio, cantidad_paginas);
}

void compactacion(){
	printf("INICIANDO COMPACTACION \n");
	usleep(RETARDO_COMPACTACION * 1000);
	int hayPagOcupadasLuegoDeUnaLibre = 1;
	while (hayPagOcupadasLuegoDeUnaLibre == 1) {
		bool primeraLibre(void * elemento) {
			t_controlPaginas * pagina = (t_controlPaginas *) elemento;
			return pagina->libre == true;
		}

		bool primeraOcupada(void * elemento) {
			t_controlPaginas * pagina = (t_controlPaginas *) elemento;
			return pagina->libre == false;
		}

		t_controlPaginas * primerPaginaLibre = list_find(paginasSWAP, primeraLibre);
		t_controlPaginas * primerPaginaOcupada = list_find(paginasSWAP, primeraOcupada);

		t_controlPaginas * primerPaginaOcupadaLuegoDeUnaLibre = NULL;
		void primerPaginaOcupadaLuegoDeLaLibre (void* elemento){
			t_controlPaginas * pagina = (t_controlPaginas *) elemento;
			if (pagina->posicion > primerPaginaLibre->posicion){
				if (pagina->libre == false){
					if (primerPaginaOcupadaLuegoDeUnaLibre == NULL){
						primerPaginaOcupadaLuegoDeUnaLibre = pagina;
					}
				}
			}
		}

		list_iterate(paginasSWAP, primerPaginaOcupadaLuegoDeLaLibre);

		if (primerPaginaOcupadaLuegoDeUnaLibre != NULL){
			t_controlPaginas * pagVacia = primerPaginaOcupada;
			pagVacia->libre = true;
			pagVacia->pid = -1;

			escribirPagina(primerPaginaLibre->posicion, discoParaleloNoVirtualMappeado + primerPaginaOcupada->posicion);
			list_replace(paginasSWAP, primerPaginaOcupada->posicion, pagVacia);

			primerPaginaOcupada->posicion = primerPaginaLibre->posicion;
			list_replace(paginasSWAP, primerPaginaLibre->posicion, primerPaginaOcupada);
		}
		else {
			printf("Terminando compactacion...\n");
			hayPagOcupadasLuegoDeUnaLibre = -1;
		}
	}
}

//int compactacion() { //Flag: 1 salio bien, 2 hubo error
//	printf("SE INVOCO AL MODULO DE COMPACTACION... \n");
//	printf("INICIANDO COMPACTACION \n");
//	int resultado = -1;
//	long int inicioOcupada, finOcupada;
//	long int inicioLibre, finLibre;
//	int primerPaginaOcupada;
//	int primerPaginaLibre;
//	do {
//		primerPaginaLibre = ultimaPagLibre();
//		primerPaginaOcupada = primerPaginaOcupadaLuegoDeUnaLibre();
//		if (primerPaginaOcupada == -1) {
//			printf(
//					"No hay mas paginas que compactar, finalizando correctamente. \n");
//			break;
//		}
//		leerPagina(primerPaginaOcupada, &inicioOcupada, &finOcupada);
//		leerPagina(primerPaginaLibre, &inicioLibre, &finLibre);
//		printf("La pagina ocupada %d pasara a la pagina %d libre \n",
//				primerPaginaOcupada, primerPaginaLibre);
//		memcpy(discoParaleloNoVirtualMappeado + inicioLibre,
//				discoParaleloNoVirtualMappeado + inicioOcupada, TAMANIO_PAGINA);
//		paginasSWAP[primerPaginaOcupada].ocupada = 0;
//		paginasSWAP[primerPaginaLibre].idProcesoQueLoOcupa =
//				paginasSWAP[primerPaginaOcupada].idProcesoQueLoOcupa;
//		paginasSWAP[primerPaginaOcupada].idProcesoQueLoOcupa = -1;
//		paginasSWAP[primerPaginaLibre].ocupada = 1;
//	} while (hayPaginasOcupadasLuegoDeLaUltimaLibre());
//	resultado = 1;
//	printf("Terminando compactacion...\n");
//	usleep(RETARDO_COMPACTACION * 1000);
//	return resultado;
//}

