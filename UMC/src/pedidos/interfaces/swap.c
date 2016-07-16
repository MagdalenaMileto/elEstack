/*
 * swap.c
 *
 *  Created on: 19/5/2016
 *      Author: utnso
 */
#include "swap.h"

bool swap_inicializar_proceso(int pid, int cantidad_paginas, char * codigo) {

	pthread_mutex_lock(&semaforo_mutex_swap);
	log_info(log,
			"\x1b[36mSe envia el pedido de inicialización del pid %d (%d paginas) al swap.\n\x1b[0m",
			pid, cantidad_paginas);

	int tamanio_codigo = string_length(codigo);
	int tamanio_paquete = (sizeof(int) * 2) + tamanio_codigo;
	void * data = malloc(tamanio_paquete);

	memcpy(data, &pid, sizeof(int));
	memcpy(data + sizeof(int), &cantidad_paginas, sizeof(int));
	memcpy(data + (sizeof(int) * 2), codigo, tamanio_codigo);

	enviar(socket_swap, SWAP_INICIALIZAR, tamanio_paquete, data);

	t_paquete * respuesta = recibir(socket_swap);

	bool resultado = respuesta->codigo_operacion == SWAP_EXITO;

	log_info(log,
			"\x1b[36mRespuesta del swap recibida, el proceso se inicializo con exito\n\x1b[0m");

	liberar_paquete(respuesta);

	free(data);

	pthread_mutex_unlock(&semaforo_mutex_swap);

	return resultado;

}

void swap_finalizar_proceso(int pid) {

	pthread_mutex_lock(&semaforo_mutex_swap);
	log_info(log,
			"\x1b[36mSe envia el pedido de finalización al swap\n\x1b[0m");
	enviar(socket_swap, SWAP_FINALIZAR, sizeof(int), (void *) &pid);

	log_info(log,
			"\x1b[36mSe envia exitosamente la peticion de finalizacion del proceso %d al swap\n\x1b[0m",
			pid);

	pthread_mutex_unlock(&semaforo_mutex_swap);
}

void * swap_leer(int pid, int numero_pagina) {

	pthread_mutex_lock(&semaforo_mutex_swap);
	log_info(log,
			"Se envia el pedido de lectura al swap con el pid %d y pagina %d\n",
			pid, numero_pagina);
	int * pid_proceso = malloc(sizeof(int));
	*pid_proceso = pid;

	pthread_mutex_lock(&semaforo_mutex_stats_swap);
	list_add(lecturas_swap, pid_proceso);
	pthread_mutex_unlock(&semaforo_mutex_stats_swap);

	int tamanio_paquete = sizeof(int) * 2;
	void * data = malloc(tamanio_paquete);

	memcpy(data, &pid, sizeof(int));
	memcpy(data + sizeof(int), &numero_pagina, sizeof(int));

	enviar(socket_swap, SWAP_LEER, tamanio_paquete, data);

	t_paquete * paquete = recibir(socket_swap);

	log_info(log, "El contenido de la pagina es %s\n", (char *) paquete->data);

	pthread_mutex_unlock(&semaforo_mutex_swap);

	return paquete->data;

}

void swap_escribir(t_entrada_tabla_de_paginas * entrada) {

	pthread_mutex_lock(&semaforo_mutex_swap);
	log_info(log, "Se envia el pedido de escritura al swap");
	int * pid_proceso = malloc(sizeof(int));
	*pid_proceso = entrada->pid;

	pthread_mutex_lock(&semaforo_mutex_stats_swap);
	list_add(escrituras_swap, pid_proceso);
	pthread_mutex_unlock(&semaforo_mutex_stats_swap);

	int tamanio = tamanio_marco + sizeof(int) * 2;

	void * data = malloc(tamanio);

	int marco = entrada->marco;

	memcpy(data, &entrada->pid, sizeof(int));
	memcpy(data + sizeof(int), &entrada->pagina, sizeof(int));
	memcpy(data + sizeof(int) * 2, memoria + marco * tamanio_marco,
			tamanio_marco);

	log_info(log, "Se escribe al swap la pagina %d del proceso %d.\n",
			entrada->pagina, entrada->pid);

	enviar(socket_swap, SWAP_ESCRIBIR, tamanio, data);

	pthread_mutex_unlock(&semaforo_mutex_swap);

	free(data);

}
