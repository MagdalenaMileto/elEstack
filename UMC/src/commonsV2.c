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

