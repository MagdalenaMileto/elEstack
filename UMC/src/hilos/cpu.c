/*
 * cpu.c
 *
 *  Created on: 29/4/2016
 *      Author: utnso
 */

#include "cpu.h"

void atender_cpu(void * parametro_hilo) {

	int socket_cpu = *(int *) parametro_hilo;
	bool cerrar = false;
	t_paquete * paquete_nuevo;

	int numero_pagina, offset, tamanio, pid_actual;

	while (!cerrar) {
		paquete_nuevo = recibir(socket_cpu);

		pthread_mutex_lock(&semaforo_mutex_cpu);

		switch (paquete_nuevo->codigo_operacion) {

		case LEER:

			memcpy(&numero_pagina, paquete_nuevo->data, sizeof(int));
			memcpy(&offset, paquete_nuevo->data + sizeof(int), sizeof(int));
			memcpy(&tamanio, paquete_nuevo->data + sizeof(int) * 2,
					sizeof(int));

			proceso_actual = pid_actual;

			void * contenido = leer_una_pagina(numero_pagina, offset, tamanio);

			enviar(socket_cpu, 6, tamanio, contenido);

			break;

		case ESCRIBIR:

			proceso_actual = pid_actual;

			break;

		case CAMBIAR_PROCESO:

			memcpy(&pid_actual, paquete_nuevo->data, sizeof(int));

			break;

		case CERRAR:

			cerrar = true;

			break;

		}

		liberar_paquete(paquete_nuevo);
		pthread_mutex_unlock(&semaforo_mutex_cpu);
	}

	close(socket_cpu);
}
