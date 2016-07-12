/*
 * tlb.c
 *
 *  Created on: 2/6/2016
 *      Author: utnso
 */
#include "tlb.h"

t_entrada_tabla_de_paginas * buscar_tlb(int pid, int numero_pagina) {

	bool filtrar_por_proceso_y_pagina(void * elemento_de_la_lista) {

		t_entrada_tabla_de_paginas * entrada = elemento_de_la_lista;

		return entrada->pagina == numero_pagina && entrada->pid == pid;
	}

	pthread_mutex_lock(&semaforo_mutex_tlb);
	t_entrada_tabla_de_paginas * resultado = list_find(tlb,
			filtrar_por_proceso_y_pagina);
	pthread_mutex_unlock(&semaforo_mutex_tlb);

	int * pid_pedido = malloc(sizeof(int));

	memcpy(pid_pedido, &pid, sizeof(int));

	if (resultado) {

		log_info(log, "Acierto en la TLB.\n");

		pthread_mutex_lock(&semaforo_mutex_stats_tlb);
		list_add(aciertos_tlb, pid_pedido);
		pthread_mutex_unlock(&semaforo_mutex_stats_tlb);

		pthread_mutex_lock(&semaforo_mutex_tlb);
		resultado->ultima_vez_usado = numero_operacion();
		pthread_mutex_unlock(&semaforo_mutex_tlb);

	} else {

		log_info(log, "No se encuentra en la TLB.\n");

		pthread_mutex_lock(&semaforo_mutex_stats_tlb);
		list_add(fallos_tlb, pid_pedido);
		pthread_mutex_unlock(&semaforo_mutex_stats_tlb);

		resultado = buscar_pagina_tabla_de_paginas(pid, numero_pagina);

		pthread_mutex_lock(&semaforo_mutex_tlb);
		agregar_entrada_tlb(resultado);
		pthread_mutex_unlock(&semaforo_mutex_tlb);
	}

	return resultado;

}

void agregar_entrada_tlb(t_entrada_tabla_de_paginas * entrada) {

	entrada->ultima_vez_usado = numero_operacion();

	if (hay_entradas_tlb_disponibles()) {

		list_add(tlb, entrada);

		log_info(log,
				"Se agrega entrada en la TLB sin necesidad del algoritmo de reemplazo.\n");
	} else {

		remplazo_lru(entrada);
	}
}

bool hay_entradas_tlb_disponibles() {

	int cantidad_disponible = entradas_TLB - list_size(tlb);

	return cantidad_disponible > 0;

}

void remplazo_lru(t_entrada_tabla_de_paginas * entrada) {

	bool orden_segun_menos_uso(void * elem1, void * elem2) {

		t_entrada_tabla_de_paginas * entrada1 =
				(t_entrada_tabla_de_paginas*) elem1;
		t_entrada_tabla_de_paginas * entrada2 =
				(t_entrada_tabla_de_paginas*) elem2;

		return entrada1->ultima_vez_usado < entrada2->ultima_vez_usado;

	}

	list_sort(tlb, orden_segun_menos_uso);

	list_remove(tlb, 0);

	log_info(log,
			"Se remueve de su marco el proceso con la referencia mas vieja.\n");

	list_add(tlb, entrada);

	log_info(log,
			"Se agrega la nueva entrada en la TLB, esta es la pagina %d del proceso %d y su marco es %d\n",
			entrada->pagina, entrada->pid, entrada->marco);

}

int numero_operacion() {
	int numero = numero_operacion_actual;
	numero_operacion_actual++;

	return numero;

}

void eliminar_proceso_tlb( pid) {

	bool lambda_coincide_pid(void * elemento) {

		t_entrada_tabla_de_paginas * entrada =
				(t_entrada_tabla_de_paginas*) elemento;

		return entrada->pid == pid;

	}

	void no_hacer_nada(void * elemento) {

	}

	pthread_mutex_lock(&semaforo_mutex_tlb);

	remove_and_destroy_all_such_that(tlb, lambda_coincide_pid, no_hacer_nada);

	log_info(log, "Se eliminaron las entradas de la tlb del proceso %d.\n",
			pid);

	pthread_mutex_unlock(&semaforo_mutex_tlb);

}

bool tlb_habilitada() {
	return entradas_TLB != 0;
}
