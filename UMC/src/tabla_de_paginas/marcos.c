/*
 * marcos.c
 *
 *  Created on: 5/6/2016
 *      Author: utnso
 */

#include "marcos.h"

int marco_nuevo() {

	if (tiene_cantidad_maxima_marcos_asignados(proceso_actual)) {

		//TODO desalojar
		return 2;

	} else {

		bool marco_disponible(void * entrada) {

			t_control_marco * marco = (t_control_marco *) entrada;

			return marco->disponible;
		}

		t_control_marco * marco_libre = list_find(control_de_marcos,
				marco_disponible);

		marco_libre->disponible = false;

		return marco_libre->numero;

	}
}

bool tiene_cantidad_maxima_marcos_asignados( proceso_actual) {

	bool coincide_pid_y_esta_presente(void * elemento) {

		t_entrada_tabla_de_paginas * entrada =
				(t_entrada_tabla_de_paginas *) elemento;

		return entrada->pid == proceso_actual && entrada->presencia;

	}

	int marcos_utilizados = list_count_satisfying(tabla_de_paginas,
			coincide_pid_y_esta_presente);

	return marcos_utilizados == cantidad_maxima_marcos;
}

void inicializar_marcos() {

	control_de_marcos = list_create();

	int var;
	for (var = 0; var < cantidad_marcos; ++var) {

		t_control_marco * entrada = malloc(sizeof(t_control_marco));

		entrada->numero = var;
		entrada->disponible = true;

		list_add(control_de_marcos, entrada);

	}
}
