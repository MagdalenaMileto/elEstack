/*
 * commonsV2.c
 *
 *  Created on: 19/5/2016
 *      Author: utnso
 */

#include "commonsV2.h"

void remove_and_destroy_all_such_that(t_list * lista, bool (*condicion)(void*),
		void (*custom_free)(void*)) {

	int cantidad_elementos = list_count_satisfying(lista, condicion);

	int aux;
	for (aux = 0; aux < cantidad_elementos; aux++) {

		list_remove_and_destroy_by_condition(lista, condicion, custom_free);

	}

}

void * head(t_list * lista) {

	return list_get(lista, 0);
}

t_list * circular_list_starting_with(t_list * lista, void * element_address) {

	t_list * resultado = list_create();

	list_add(resultado, element_address);

	int posicion_elemento = index_of(lista, element_address);

	list_add_all(resultado, list_take(lista, posicion_elemento));

	list_add_all(resultado, list_drop(lista, posicion_elemento + 1));

	return resultado;

}

int index_of(t_list * lista, void * element_address) {

	int posicion_elemento = -1;
	int posicion_actual = 0;

	void conseguir_posicion_elemento(void * elemento) {

		if (element_address == elemento) {

			posicion_elemento = posicion_actual;
		} else {

			posicion_actual++;
		}
	}

	list_iterate(lista, conseguir_posicion_elemento);

	return posicion_elemento;

}

t_list * list_drop(t_list * lista, int cantidad) {
	if (cantidad < 0) {

		return lista;
	}

	bool posteriores_a(void * elemento) {

		return index_of(lista, elemento) > cantidad - 1;

	}

	return list_filter(lista, posteriores_a);

}

