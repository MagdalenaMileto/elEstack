/*
 * marcos.c
 *
 *  Created on: 5/6/2016
 *      Author: utnso
 */

#include "marcos.h"

void marco_nuevo(t_entrada_tabla_de_paginas * entrada_que_necesita_marco) {

	log_info(log,
			"Ingreso en la funcion marco nuevo, procedo a chequear si estan asignados todos los marcos");

	if (tiene_cantidad_maxima_marcos_asignados(
			entrada_que_necesita_marco->pid)) {

		log_info(log,
				"Se llego a la capacidad maxima de marcos, se inicia el algoritmo de reemplazo.");

		algoritmo_remplazo(entrada_que_necesita_marco,
				entrada_que_necesita_marco->pid);

		log_info(log,
				"El marco se reemplaza correctamente, se procede a activar la presencia de la pagina.");

		entrada_que_necesita_marco->presencia = true;

		log_info(log,
				"Se reemplaza exitosamente y se asigna la pagina al marco correspondiente.");

	} else {

		log_info(log,
				"Hay marcos disponibles, por ende se le asigna a la pagina una nueva entrada.");

		bool marco_disponible(void * entrada) {

			t_control_marco * marco = (t_control_marco *) entrada;

			return marco->disponible;

		}

		t_control_marco * marco_libre = list_find(control_de_marcos,
				marco_disponible);

		log_info(log, "Encuentro marco disponible %d", marco_libre);

		marco_libre->disponible = false;

		log_info(log, "El marco seleccionado ya no esta disponible");

		entrada_que_necesita_marco->marco = marco_libre->numero;

		t_list * lista_clock = lista_circular_clock(tabla_de_paginas,
				entrada_que_necesita_marco->pid);

		if (list_is_empty(lista_clock)) {
			entrada_que_necesita_marco->puntero = true;
		}

		entrada_que_necesita_marco->presencia = true;

		log_info(log, "Se asigna el marco exitosamente.");
	}
}

bool tiene_cantidad_maxima_marcos_asignados(int pid) {

	bool coincide_pid_y_esta_presente(void * elemento) {

		t_entrada_tabla_de_paginas * entrada =
				(t_entrada_tabla_de_paginas *) elemento;

		return entrada->pid == pid && entrada->presencia;

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

void algoritmo_remplazo(t_entrada_tabla_de_paginas * entrada_sin_marco, int pid) {

	if (strcmp(algoritmo, "CLOCK")) {

		log_info(log, "Inicio del algoritmo de reemplazo Clock");

		t_list * lista_clock = lista_circular_clock(tabla_de_paginas, pid);

		bool buscar_victima_y_modificar_uso(void * elemento) {

			t_entrada_tabla_de_paginas * entrada =
					(t_entrada_tabla_de_paginas *) elemento;

			if (entrada->uso) {

				entrada->uso = false;
				return false;

			} else {

				return true;
			}

		}

		t_entrada_tabla_de_paginas * victima = list_find(lista_clock,
				buscar_victima_y_modificar_uso);

		if (victima == NULL) {

			victima = list_find(lista_clock, buscar_victima_y_modificar_uso);

		}

		if (victima->modificado) {

			swap_escribir(victima);
			victima->modificado = false;
		}

		entrada_sin_marco->marco = victima->marco;

		entrada_sin_marco->uso = true;

		entrada_sin_marco->presencia = true;
		victima->presencia = false;

		avanzar_victima(lista_clock, entrada_sin_marco);

	}

	if (STRCMP(algoritmo, "CLOCK-M")) {

		log_info(log, "Inicio del algoritmo de reemplazo Clock Modificado");

		t_list * lista_clock_modificado = lista_circular_clock(tabla_de_paginas,
				pid);

		bool buscar_victima_sin_modificar_uso(void * elemento) {

			t_entrada_tabla_de_paginas * entrada =
					(t_entrada_tabla_de_paginas *) elemento;

			if (!entrada->uso && !entrada->modificado) {

				return true;

			} else {

				return false;
			}

		}

		bool buscar_victima_y_modificar_uso_2(void * elemento) {

			t_entrada_tabla_de_paginas * entrada =
					(t_entrada_tabla_de_paginas *) elemento;

			if (!entrada->uso && entrada->modificado) {

				return true;

			} else {

				entrada->uso = false;
				return false;
			}

		}

		t_entrada_tabla_de_paginas * victima = list_find(lista_clock_modificado,
				buscar_victima_sin_modificar_uso);

		if (victima == NULL) {

			log_info(log,
					"No encontro victima buscando (0,0), empieza a buscar (0,1) y a poner el bit de uso en 0 de ser necesario.");

			victima = list_find(lista_clock_modificado,
					buscar_victima_y_modificar_uso_2);

			if (victima == NULL) {

				log_info(log,
						"No encontro victima buscando (0,1), vuelve a buscar (0,0).");

				victima = list_find(lista_clock_modificado,
						buscar_victima_sin_modificar_uso);
			}

		}

		if (victima->modificado) {

			swap_escribir(victima);
			victima->modificado = false;
		}

		entrada_sin_marco->marco = victima->marco;

		entrada_sin_marco->uso = true;

		entrada_sin_marco->presencia = true;
		victima->presencia = false;

		avanzar_victima(lista_clock_modificado, entrada_sin_marco);

	} else {
		error_show("No se ingreso el algoritmo correspondiente");
		exit(1);
	}
}

t_list * lista_circular_clock(t_list * lista, int pid) {

	bool coincide_pid_y_presencia(void * elemento) {

		t_entrada_tabla_de_paginas * entrada =
				(t_entrada_tabla_de_paginas *) elemento;

		return entrada->pid == pid && entrada->presencia;
	}

	t_list * entradas_presentes_proceso = list_filter(lista,
			coincide_pid_y_presencia);

	bool orden_menor_marco_primero(void * elem1, void * elem2) {

		t_entrada_tabla_de_paginas * entrada1 =
				(t_entrada_tabla_de_paginas *) elem1;

		t_entrada_tabla_de_paginas * entrada2 =
				(t_entrada_tabla_de_paginas *) elem2;

		return entrada1->marco < entrada2->marco;

	}

	if (list_is_empty(entradas_presentes_proceso)) {

		return entradas_presentes_proceso;

	} else {

		list_sort(entradas_presentes_proceso, orden_menor_marco_primero);

		bool es_puntero(void * elemento) {

			t_entrada_tabla_de_paginas * entrada =
					(t_entrada_tabla_de_paginas *) elemento;

			return entrada->puntero;
		}

		t_entrada_tabla_de_paginas * puntero = list_find(
				entradas_presentes_proceso, es_puntero);

		return circular_list_starting_with(entradas_presentes_proceso, puntero);
	}
}

void avanzar_victima(t_list * lista_clock,
		t_entrada_tabla_de_paginas * entrada_con_marco_nuevo) {

	t_entrada_tabla_de_paginas * puntero_viejo = head(lista_clock);

	if (de_una_entrada(lista_clock)) {

		entrada_con_marco_nuevo->puntero = true;

	} else {

		t_entrada_tabla_de_paginas * nuevo_puntero = list_get(lista_clock, 1);

		nuevo_puntero->puntero = true;

	}

	puntero_viejo->puntero = false;

}

bool de_una_entrada(t_list * lista) {

	return list_size(lista) == 1;

}

void escribir_marco(int marco, int offset, int tamanio, void * contenido) {

	int desplazamiento = marco * tamanio_marco;

	memcpy(memoria + desplazamiento + offset, contenido, tamanio);

	log_info(log, "El marco se escribe con exito.");

}

