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

	char * buffer;

	while (!cerrar) {
		paquete_nuevo = recibir(socket_cpu);

		pthread_mutex_lock(&semaforo_mutex_cpu);

		proceso_actual = pid_actual;

		switch (paquete_nuevo->codigo_operacion) {

		case LEER:

			memcpy(&numero_pagina, paquete_nuevo->data, sizeof(int));
			memcpy(&offset, paquete_nuevo->data + sizeof(int), sizeof(int));
			memcpy(&tamanio, paquete_nuevo->data + sizeof(int) * 2,
					sizeof(int));

			void * contenido = leer_una_pagina(numero_pagina, offset, tamanio);

			enviar(socket_cpu, 6, tamanio, contenido);

			log_info(log, "Se envia con exito a la CPU.");

			break;

		case ESCRIBIR:

			memcpy(&numero_pagina, paquete_nuevo->data, sizeof(int));
			memcpy(&offset, paquete_nuevo->data + sizeof(int), sizeof(int));
			memcpy(&tamanio, paquete_nuevo->data + sizeof(int) * 2,
					sizeof(int));

			buffer = malloc(tamanio);

			memcpy(buffer, paquete_nuevo->data + sizeof(int) * 3, tamanio);

			escribir_una_pagina(numero_pagina, offset, tamanio, buffer);

			free(buffer);

			break;

		case CAMBIAR_PROCESO:

			log_info(log, "Entro a cambiar proceso");

			memcpy(&pid_actual, paquete_nuevo->data, sizeof(int));

			log_info(log, "Se cambio el proceso actual a %d.", proceso_actual);

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
