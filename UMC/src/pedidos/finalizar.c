#include "finalizar.h"

void finalizar_proceso(int pid) {

	log_info(log, "Se envia una peticion para finalizar el proceso %id", pid);

	eliminar_proceso_tlb(pid);

	eliminar_proceso_tabla_de_paginas(pid);

	swap_finalizar_proceso(pid);

	bool coincide_pid(void * elemento) {

		int * proceso = (int *) elemento;

		return *proceso == proceso_actual;
	}

	int cantidad_aciertos, cantidad_fallas, cantidad_lecturas,
			cantidad_escrituras;

	cantidad_aciertos = list_count_satisfying(aciertos_tlb, coincide_pid);

	cantidad_fallas = list_count_satisfying(fallos_tlb, coincide_pid);

	log_info(log, "Este proceso tuvo %d aciertos y %d fallas a la tlb",
			cantidad_aciertos, cantidad_fallas);

	cantidad_lecturas = list_count_satisfying(lecturas_swap, coincide_pid);
	cantidad_escrituras = list_count_satisfying(escrituras_swap, coincide_pid);

	log_info(log, "Este proceso tuvo %d escrituras y %d lecturas al swap",
			cantidad_escrituras, cantidad_lecturas);
}

