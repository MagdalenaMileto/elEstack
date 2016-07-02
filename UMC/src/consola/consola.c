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
	while (1) {
		getline(&comando_a_realizar, &tamanio_maximo, stdin);
		{

			int tamanio = string_length(comando_a_realizar);

			if (tamanio != 0) {
				comando_a_realizar[tamanio - 1] = '\0';
			} else {
				comando_a_realizar = "\0";
			}

			pthread_mutex_lock(&semaforo_mutex_cpu);

			char * posible_retardo = string_substring_until(comando_a_realizar,
					8);
			char * posible_dump_parcial = string_substring_until(
					comando_a_realizar, 5);
			char * posible_flush_memoria = string_substring_until(
					comando_a_realizar, 13);

			if (string_equals_ignore_case(posible_retardo, "retardo ")) {

				char * resto = string_substring_from(comando_a_realizar, 8);

				if (isNumber(resto)) {

					int pid = atoi(resto);
					cambiar_retardo(pid);
				} else {

					free(resto);
					goto error;
				}

			} else if (string_equals_ignore_case(comando_a_realizar, "dump")) {

				dump_total();

			} else if (string_equals_ignore_case(posible_dump_parcial,
					"dump ")) {

				char * resto = string_substring_from(comando_a_realizar, 5);

				if (isNumber(resto)) {
					int pid = atoi(resto);
					dump_proceso(pid);
				} else {
					free(resto);
					goto error;
				}

			} else if (string_equals_ignore_case(posible_flush_memoria,
					"flush memory ")) {

				char * resto = string_substring_from(comando_a_realizar, 13);

				if (isNumber(resto)) {
					int pid = atoi(resto);
					flush_memory(pid);
				} else {
					free(resto);
					goto error;
				}

			} else if (string_equals_ignore_case(comando_a_realizar,
					"flush tlb")) {
				flush_tlb();
			} else {

				free(posible_flush_memoria);
				free(posible_dump_parcial);
				free(posible_retardo);

				error: printf("El comando \"%s\" no existe.\n",
						comando_a_realizar);
			}

		}
		pthread_mutex_unlock(&semaforo_mutex_cpu);
	}

}

void cambiar_retardo(int retardo_numerico) {

	log_info(log, "El retardo pasa de %d a %d milisegundos", retardo,
			retardo_numerico);
	retardo = retardo_numerico;
}

void flush_tlb() {

	log_info(log, "La TLB tiene %d entradas\n", list_size(tlb));
	log_info(log, "Se flushea la TLB\n");
	list_clean(tlb);
	log_info(log, "La TLB paso a tener %d entradas\n", list_size(tlb));

}

void flush_memory(int proceso) {

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
		log_info(log, "La pagina %d del proceso %d se marca como modificada\n",
				proceso, entrada_tabla_paginas->pagina);

	}

	log_info(log, "Se flushea el proceso %d\n", proceso);
	log_info(log, "===== INICIO FLUSH =====\n", proceso);
	list_iterate(lista_filtrada_por_proceso, cambiar_bit);
	log_info(log, "===== FIN FLUSH =====\n", proceso);

}

void dump_total() {

	escribir_a_dump(string_from_format("\n===== INICIO DUMP =====\n"));

	list_iterate(tabla_de_paginas, dump_proceso_iterate);

	escribir_a_dump(string_from_format("\n===== FIN DUMP =====\n"));

}

void dump_proceso(int pid) {

	bool filtrar_por_proceso_dump(void * entrada) {

		t_entrada_tabla_de_paginas * entrada_tabla_paginas =
				(t_entrada_tabla_de_paginas *) entrada;

		return entrada_tabla_paginas->pid == pid;

	}

	t_list * tabla_filtrada = list_filter(tabla_de_paginas,
			filtrar_por_proceso_dump);

	escribir_a_dump(
			string_from_format("\n===== INICIO DUMP PROCESO %d =====\n", pid));

	list_iterate(tabla_filtrada, dump_proceso_iterate);

	escribir_a_dump(
			string_from_format("\n===== FIN DUMP PROCESO %d =====\n", pid));

}

void dump_entrada(t_entrada_tabla_de_paginas * entrada) {

	char * linea = string_from_format("La pagina %d del proceso %d ",
			entrada->pagina, entrada->pid);

	if (entrada->presencia) {
		string_append(&linea,
				string_from_format("esta presente en el marco %d ",
						entrada->marco));
	} else {
		string_append(&linea,
				string_from_format("no esta presente en memoria"));
	}

	string_append(&linea, ", ");

	if (entrada->modificado) {
		string_append(&linea, string_from_format("esta modificado "));
	} else {
		string_append(&linea, string_from_format("no esta modificado "));
	}

	string_append(&linea, "y ");

	if (entrada->uso) {
		string_append(&linea, string_from_format("tiene uso"));
	} else {
		string_append(&linea, string_from_format("no tiene uso"));
	}

	string_append(&linea, ".\n");

	if (entrada->presencia) {

		void * contenido = malloc(tamanio_marco);

		int desplazamiento = entrada->marco * tamanio_marco;

		memcpy(contenido, memoria + desplazamiento, tamanio_marco);

		string_append(&linea,
				string_from_format("Su contenido es: %s.\n", contenido));

	}
	printf("%s", linea);
	escribir_a_dump(linea);

}

void dump_proceso_iterate(void * elemento) {

	t_entrada_tabla_de_paginas * entrada =
			(t_entrada_tabla_de_paginas *) elemento;

	dump_entrada(entrada);
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

void escribir_a_dump(char * string) {

	int largo = string_length(string) + 1;

	FILE * dump = fopen("./memory.dump", "a");

	fwrite(string, 1, largo, dump);

	fclose(dump);

}
