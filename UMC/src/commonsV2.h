/*
 * commonsV2.h
 *
 *  Created on: 19/5/2016
 *      Author: utnso
 */

#ifndef SRC_COMMONSV2_H_
#define SRC_COMMONSV2_H_

#include "tabla_de_paginas/estructuras.h"

#include <stdbool.h>
#include <stdlib.h>

void remove_and_destroy_all_such_that(t_list * lista, bool (*condicion)(void*),
		void (*custom_free)(void*));

void * head(t_list * lista);

t_list * circular_list_starting_with(t_list * lista, void * element_address);

int index_of(t_list * lista, void * element_address);

t_list * list_drop(t_list * lista, int cantidad);


#endif /* SRC_COMMONSV2_H_ */
