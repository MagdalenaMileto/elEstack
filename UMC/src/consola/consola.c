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

	comando_a_realizar = string_drop_from_end(comando_a_realizar, 1);

	pthread_mutex_lock(&semaforo_mutex_cpu);
	char ** palabras = string_split(comando_a_realizar, " ");
	char * primer_palabra = palabras[0];

	if (string_equals_ignore_case(primer_palabra, "retardo")) {

		char * segunda_palabra = palabras[1];

		if (isNumber(segunda_palabra)) {

			int retardo_numerico = atoi(segunda_palabra);
			cambiar_retardo(retardo_numerico);

		} else {

			goto error;
		}

	} else if (string_equals_ignore_case(primer_palabra, "dump")) {

		//TODO: Dump all.

		char * segunda_palabra = palabras[1];

		if (isNumber(segunda_palabra)) {

			int proceso_a_dumpear = atoi(segunda_palabra);

			//TODO: dumpear este proceso

		} else {

			goto error;
		}

	} else if (string_equals_ignore_case(primer_palabra, "flush")) {

		char * segunda_palabra = palabras[1];

		if (string_equals_ignore_case(segunda_palabra, "tlb")) {

			vaciar_tlb();

		} else if (string_equals_ignore_case(segunda_palabra, "memory")) {

			char * tercer_palabra = palabras[2];

			if (isNumber(tercer_palabra)) {

				int pid = atoi(tercer_palabra);
				marcar_las_paginas_del_proceso_como_modificadas(pid);

			} else {
				goto error;
			}

		} else {
			goto error;
		}

	} else {

		//Comando inválido.
		error: printf("El comando \"%s\" a realizar no existe.",
				comando_a_realizar);
	}
	pthread_mutex_unlock(&semaforo_mutex_cpu);
}

void cambiar_retardo(int retardo_numerico) {

	retardo = retardo_numerico;
}

void vaciar_tlb() {

	list_clean(tlb);
}

void marcar_las_paginas_del_proceso_como_modificadas(int proceso) {

	bool filtrar_por_proceso(void * entrada) {

		t_entrada_tabla_de_paginas * entrada_tabla_paginas =
				(t_entrada_tabla_de_paginas *) entrada;

		return entrada_tabla_paginas->pid == proceso;

	}

	t_list * lista_filtrada_por_proceso = list_filter(tabla_de_paginas,
			filtrar_por_proceso);

	void cambiar_bit(void * entrada) {

		t_entrada_tabla_de_paginas * entrada_tabla_paginas =
				(t_entrada_tabla_de_paginas *) entrada;

		entrada_tabla_paginas->modificado = true;

	}

	list_iterate(lista_filtrada_por_proceso, cambiar_bit);

}

//Funciones Auxiliares:
char * string_drop_from_end(char * string, int cuantos) {

	return string_reverse(string_reverse(string) + cuantos);
}

bool isNumber(char * palabra) {

	int tamanio = string_length(palabra);

	t_list * lista_chars = list_create();

	int i;
	for (i = 0; i < tamanio; i++) {

		list_add(lista_chars, (void *) palabra[i]);

	}

	bool es_digito(void * elemento) {

		char * letra = (char *) elemento;

		return isdigit(letra);
	}

	return list_all_satisfy(lista_chars, es_digito);

}

