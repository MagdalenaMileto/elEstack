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

void remove_and_destroy_all_such_that(t_list * lista, bool (*condicion)(void*),
		void (*custom_free)(void*));

t_list * coincide_pid(t_list * lista, int pid);

#endif /* SRC_COMMONSV2_H_ */
