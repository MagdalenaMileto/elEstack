/*
 * swap.c
 *
 *  Created on: 19/5/2016
 *      Author: utnso
 */
#include "swap.h"

bool swap_inicializar_proceso(int pid, int cantidad_paginas, char * codigo) {

	int tamanio_codigo = string_length(codigo);
	int tamanio_paquete = (sizeof(int) * 2) + tamanio_codigo;
	void * data = malloc(tamanio_paquete);

	memcpy(data, &pid, sizeof(int));
	memcpy(data + sizeof(int), &cantidad_paginas, sizeof(int));
	memcpy(data + (sizeof(int) * 2), codigo, tamanio_codigo);

	enviar(socket_swap, SWAP_INICIALIZAR, tamanio_paquete, data);

	t_paquete * respuesta = recibir(socket_swap);

	bool resultado = respuesta->codigo_operacion == SWAP_EXITO;

	liberar_paquete(respuesta);
	free(data);

	return resultado;

}

void swap_finalizar_proceso(int pid) {
	enviar(socket_swap, SWAP_FINALIZAR, sizeof(int), (void *) &pid);
}

char * swap_leer(int pid, int numero_pagina) {

	int tamanio_paquete = sizeof(int) * 2;
	void * data = malloc(tamanio_paquete);

	memcpy(data, &pid, sizeof(int));
	memcpy(data + sizeof(int), &numero_pagina, sizeof(int));

	enviar(socket_swap, SWAP_LEER, tamanio_paquete, data);

	t_paquete * paquete = recibir(socket_swap);

	return (char *) paquete->data;

}

void swap_escribir(t_entrada_tabla_de_paginas * entrada){

	int tamanio = tamanio_marco + sizeof(int) * 2;

	void * data = malloc(tamanio);

	int marco = entrada->marco;

	memcpy(data, &entrada->pid, sizeof(int));
	memcpy(data + sizeof(int), &entrada->pagina, sizeof(int));
	memcpy(data + sizeof(int) * 2, memoria + marco * tamanio_marco,tamanio_marco);

	enviar(socket_swap, SWAP_ESCRIBIR,tamanio,data);

	free(data);

}
