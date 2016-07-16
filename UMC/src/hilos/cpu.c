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

		switch (paquete_nuevo->codigo_operacion) {

		case -1:
			log_info(log, "\x1b[31mEvento desconexion CPU\n\x1b[0m");

			return;
			break;

		case LEER:

			memcpy(&numero_pagina, paquete_nuevo->data, sizeof(int));
			memcpy(&offset, paquete_nuevo->data + sizeof(int), sizeof(int));
			memcpy(&tamanio, paquete_nuevo->data + sizeof(int) * 2,
					sizeof(int));

			void * contenido = leer_una_pagina(pid_actual, numero_pagina,
					offset, tamanio);

			char * texto_falla = string_from_format(
					"\x1b[31mNo hay lugar disponible!\x1b[0m");
			int tamanio_mensaje_falla = string_length(texto_falla) + 1;

			if (string_equals_ignore_case(contenido, texto_falla)) {

				enviar(socket_cpu, 7, tamanio_mensaje_falla, contenido);

				log_info(log,
						"\x1b[31mSe avisa que no hubo exito exito a la CPU, el proceso es el %d.\n\x1b[0m",
						pid_actual);

			} else {

				enviar(socket_cpu, 6, tamanio, contenido);

				log_info(log,
						"\x1b[32mSe envia con exito a la CPU el proceso %d.\n\x1b[0m",
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

			int a;
			enviar(socket_cpu, 6, sizeof(int), &a);

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
