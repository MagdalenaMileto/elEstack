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
		log_info(log,"recibi de CPU");
		log_info(log,"Codigo recibi CPU:%d",paquete_nuevo->codigo_operacion);
		switch (paquete_nuevo->codigo_operacion) {

		case -1:
			log_info(log,"Evento desconexion CPU");
			//	printf("Evento desconexion cpu\n");
			//no se si deberias hacer un free de parametro hilo u otras cosas
			return;
			break;

		case LEER:

			memcpy(&numero_pagina, paquete_nuevo->data, sizeof(int));
			memcpy(&offset, paquete_nuevo->data + sizeof(int), sizeof(int));
			memcpy(&tamanio, paquete_nuevo->data + sizeof(int) * 2,
					sizeof(int));

			void * contenido = leer_una_pagina(pid_actual, numero_pagina,
					offset, tamanio);

			char * texto_falla = string_from_format("No hay lugar disponible!");
			int tamanio_mensaje_falla = string_length(texto_falla) + 1;

			if (string_equals_ignore_case(contenido, texto_falla)) {

				enviar(socket_cpu, 7, tamanio_mensaje_falla, contenido);

				log_info(log, "Se envia sin exito a la CPU el proceso %d.\n",
						pid_actual);

			} else {

				enviar(socket_cpu, 6, tamanio, contenido);

				log_info(log, "Se envia con exito a la CPU el proceso %d.\n",
						pid_actual);

			}

			free(texto_falla);
			break;

		case ESCRIBIR:

			memcpy(&numero_pagina, paquete_nuevo->data, sizeof(int));
			memcpy(&offset, paquete_nuevo->data + sizeof(int), sizeof(int));
			memcpy(&tamanio, paquete_nuevo->data + sizeof(int) * 2,
					sizeof(int));

			buffer = malloc(tamanio);

			memcpy(buffer, paquete_nuevo->data + sizeof(int) * 3, tamanio);

			escribir_una_pagina(pid_actual, numero_pagina, offset, tamanio,
					buffer);

			free(buffer);

			break;

		case CAMBIAR_PROCESO:

			log_info(log, "Entro a cambiar proceso.\n");

			memcpy(&pid_actual, paquete_nuevo->data, sizeof(int));

			log_info(log, "Se cambio el proceso actual a %d.\n", pid_actual);

			break;

		case CERRAR:

			cerrar = true;

			break;

		}

		liberar_paquete(paquete_nuevo);
	}

	close(socket_cpu);
}
