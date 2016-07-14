#include "finalizar.h"

void finalizar_proceso(int pid) {

	log_info(log, "Se envia una peticion para finalizar el proceso %d\n", pid);

	swap_finalizar_proceso(pid);

	eliminar_proceso_tlb(pid);

	pthread_mutex_lock(&semaforo_mutex_tabla_de_paginas);
	eliminar_proceso_tabla_de_paginas(pid);
	pthread_mutex_unlock(&semaforo_mutex_tabla_de_paginas);


	bool coincide_pid(void * elemento) {

		int * proceso = (int *) elemento;

		return *proceso == pid;
	}

	int cantidad_aciertos, cantidad_fallas, cantidad_lecturas,
			cantidad_escrituras;

	pthread_mutex_lock(&semaforo_mutex_stats_tlb);
	cantidad_aciertos = list_count_satisfying(aciertos_tlb, coincide_pid);
	cantidad_fallas = list_count_satisfying(fallos_tlb, coincide_pid);
	pthread_mutex_unlock(&semaforo_mutex_stats_tlb);

	log_info(log, "Este proceso tuvo %d aciertos y %d fallas a la tlb.\n",
			cantidad_aciertos, cantidad_fallas);

	pthread_mutex_lock(&semaforo_mutex_stats_swap);
	cantidad_lecturas = list_count_satisfying(lecturas_swap, coincide_pid);
	cantidad_escrituras = list_count_satisfying(escrituras_swap, coincide_pid);
	pthread_mutex_unlock(&semaforo_mutex_stats_swap);

	log_info(log, "Este proceso tuvo %d escrituras y %d lecturas al swap.\n",
			cantidad_escrituras, cantidad_lecturas);
}

