/*
 * nucleo.c
 *
 *  Created on: 29/4/2016
 *      Author: utnso
 */
#include "nucleo.h"

void atender_nucleo() {

	t_paquete * paquete_nuevo;

	int pid, paginas_requeridas, tamanio_codigo;
	char * codigo;

	while (1) {

		paquete_nuevo = recibir(socket_nucleo);

		switch (paquete_nuevo->codigo_operacion) {
		case INICIALIZAR:

			memcpy(&pid, paquete_nuevo->data, sizeof(int));
			memcpy(&paginas_requeridas, paquete_nuevo->data + sizeof(int),
					sizeof(int));

			log_info(log,
					"x1b[34mLlega un nuevo proceso, las paginas requeridas son %d.\n\x1b[0m",
					paginas_requeridas);

			memcpy(&tamanio_codigo, paquete_nuevo->data + sizeof(int) * 2,
					sizeof(int));

			codigo = malloc(tamanio_codigo);

			memcpy(codigo, paquete_nuevo->data + sizeof(int) * 3,
					tamanio_codigo);

			if (puede_iniciar_proceso(pid, paginas_requeridas, codigo)) {

				inicializar_programa(pid, paginas_requeridas);

				log_info(log,
						"x1b[34mSe pudo inicializar el proceso con el pid %d.\n\x1b[0m",
						pid);

				enviar(socket_nucleo, EXITO, sizeof(int), &pid);

			} else {

				log_info(log,
						"x1b[34mNo se pudo inicializar el proceso con el pid %d.\n\x1b[0m",
						pid);

				enviar(socket_nucleo, FRACASO, sizeof(int), &pid);
			}

			free(codigo);

			break;

		case FINALIZAR:

			pid = *(int *) paquete_nuevo->data;

			finalizar_proceso(pid);

			break;

		}

		liberar_paquete(paquete_nuevo);

	}
}
