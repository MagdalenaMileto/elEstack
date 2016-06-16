/*
 * consola.c
 *
 *  Created on: 15/6/2016
 *      Author: utnso
 */

#include "consola.h"

void esperar_comando(void * parametros) {

	char * comando_a_realizar = malloc(50);
	size_t tamanio_maximo = 50;

	getline(&comando_a_realizar, &tamanio_maximo, stdin);

	char ** palabras = string_split(comando_a_realizar, " ");
	char * primer_palabra = palabras[0];
	char * segunda_palabra = string_drop_from_end(palabras[1], 1);

	if (string_equals_ignore_case(primer_palabra, "retardo")) {

		int retardo_numerico = atoi(segunda_palabra);
		cambiar_retardo(retardo_numerico);

	} else if (string_equals_ignore_case(primer_palabra, "dump")) {

		//Dump.

	} else if (string_equals_ignore_case(primer_palabra, "flush")) {

		if (string_equals_ignore_case(segunda_palabra, "tlb")) {

			vaciar_tlb();

		} else if (string_equals_ignore_case(segunda_palabra, "memory")) {

			//Memory.

		} else {

			printf("El comando a realizar no existe.");
		}

	} else {

		//Comando inválido.
	}
}

void cambiar_retardo(int retardo_numerico) {

	retardo = retardo_numerico;
}

void vaciar_tlb(){

	pthread_mutex_lock(&semaforo_mutex_consola);
	list_clean(tlb);
}


//Funciones Auxiliares:
char * string_drop_from_end(char * string, int cuantos) {

	return string_reverse(string_reverse(string) + cuantos);
}
