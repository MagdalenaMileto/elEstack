/*
 * info.c
 *
 *  Created on: 1/7/2016
 *      Author: utnso
 */
#include "info.h"

void mostrar_estado_interno() {

	while (1) {

		sleep(intervalo_info);

		int cantidad_aciertos, cantidad_fallas;

		pthread_mutex_lock(&semaforo_mutex_stats_tlb);
		cantidad_aciertos = list_size(aciertos_tlb);
		cantidad_fallas = list_size(fallos_tlb);
		pthread_mutex_unlock(&semaforo_mutex_stats_tlb);

		int cantidad_total = cantidad_fallas + cantidad_aciertos;

		printf("La TLB tiene %d aciertos y %d fallos sobre %d pedidos \n\n",
				cantidad_aciertos, cantidad_fallas, cantidad_total);

		if (cantidad_total != 0) {

			printf("La tasa	de acierto es %.1f\%\n\n",
					(cantidad_aciertos / (float) cantidad_total) * 100);

		}

	}
}

