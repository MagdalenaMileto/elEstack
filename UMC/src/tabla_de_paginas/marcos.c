/*
 * marcos.c
 *
 *  Created on: 5/6/2016
 *      Author: utnso
 */

#include "marcos.h"

void marco_nuevo(t_entrada_tabla_de_paginas * entrada_que_necesita_marco) {

	log_info(log,
			"Ingreso en la funcion marco nuevo, procedo a chequear si estan asignados todos los marcos\n");

	if (tiene_cantidad_maxima_marcos_asignados(
			entrada_que_necesita_marco->pid)) {

		pthread_mutex_unlock(&semaforo_mutex_marcos);

		log_info(log,
				"Se llego a la capacidad maxima de marcos, se inicia el algoritmo de reemplazo.\n");

		algoritmo_remplazo(entrada_que_necesita_marco,
				entrada_que_necesita_marco->pid);

		log_info(log,
				"Se reemplaza exitosamente y se asigna la pagina %d del proceso %d al marco %d.\n",
				entrada_que_necesita_marco->pagina,
				entrada_que_necesita_marco->pid,
				entrada_que_necesita_marco->marco);

		entrada_que_necesita_marco->presencia = true;

	} else {

		log_info(log,
				"Hay marcos disponibles, por ende se le asigna a la pagina una nueva entrada.\n");

		bool marco_disponible(void * entrada) {

			t_control_marco * marco = (t_control_marco *) entrada;

			return marco->disponible;

		}

		t_control_marco * marco_libre = list_find(control_de_marcos,
				marco_disponible);

		log_info(log, "Encuentro marco disponible %d.\n", marco_libre->numero);

		marco_libre->disponible = false;

		log_info(log, "El marco seleccionado ya no esta disponible.\n");

		pthread_mutex_unlock(&semaforo_mutex_marcos);

		entrada_que_necesita_marco->presencia = true;

		entrada_que_necesita_marco->marco = marco_libre->numero;

		bool coincide_pid_y_esta_presente(void * elemento) {
			t_entrada_tabla_de_paginas * entrada =
					(t_entrada_tabla_de_paginas *) elemento;
			return entrada->presencia
					&& entrada->pid == entrada_que_necesita_marco->pid;
		}

		pthread_mutex_lock(&semaforo_mutex_tabla_de_paginas);
		t_list * paginas_presentes = list_filter(tabla_de_paginas,
				coincide_pid_y_esta_presente);
		pthread_mutex_unlock(&semaforo_mutex_tabla_de_paginas);

		if (list_size(paginas_presentes) == 1) {
			log_info(log,
					"Es el primer marco del proceso, se lo asigna como puntero.\n");
			entrada_que_necesita_marco->puntero = true;
		} else {
			log_info(log,
					string_from_format(
							"No es el primer marco del proceso, ya ocupa %d marcos. No se le asigna el puntero\n",
							list_size(paginas_presentes)));
		}

		log_info(log,
				string_from_format("Se asigna el marco %d exitosamente.\n",
						marco_libre->numero));
	}
}

bool tiene_cantidad_maxima_marcos_asignados(int pid) {

	bool coincide_pid_y_esta_presente(void * elemento) {

		t_entrada_tabla_de_paginas * entrada =
				(t_entrada_tabla_de_paginas *) elemento;

		return (entrada->pid == pid && entrada->presencia);

	}

	bool marco_disponible(void * entrada) {

		t_control_marco * marco = (t_control_marco *) entrada;

		return marco->disponible;

	}

	pthread_mutex_lock(&semaforo_mutex_tabla_de_paginas);
	int marcos_utilizados = list_count_satisfying(tabla_de_paginas,
			coincide_pid_y_esta_presente);

	t_control_marco * marco_libre = list_find(control_de_marcos,
			marco_disponible);
	pthread_mutex_unlock(&semaforo_mutex_tabla_de_paginas);

	return (marcos_utilizados == cantidad_maxima_marcos)
			|| (marco_libre == NULL);
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

	if (string_equals_ignore_case(algoritmo, "CLOCK")) {

		log_info(log, "Inicio del algoritmo de reemplazo Clock\n");

		pthread_mutex_lock(&semaforo_mutex_tabla_de_paginas);
		t_list * lista_clock = lista_circular_clock(tabla_de_paginas, pid);
		pthread_mutex_unlock(&semaforo_mutex_tabla_de_paginas);

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

			log_info(log, "Nadie tenia uso en 0, no hay victima\n");
			victima = list_find(lista_clock, buscar_victima_y_modificar_uso);

		}
		log_info(log,
				string_from_format(
						"La victima es la pagina %d, tiene el marco %d y puntero esta en %d\n",
						victima->pagina, victima->marco, victima->puntero));

		if (victima->modificado) {

			log_info(log,
					"La victima esta modificada, se escribe en el swap.\n");

			swap_escribir(victima);
			victima->modificado = false;

		}

		entrada_sin_marco->marco = victima->marco;

		log_info(log,
				string_from_format("Se le asigna el marco %d.\n",
						victima->marco));

		entrada_sin_marco->uso = true;

		entrada_sin_marco->presencia = true;

		avanzar_victima(lista_clock, entrada_sin_marco, victima);

		log_info(log, "***********\n");
		int i;
		for (i = 0; i < list_size(lista_clock); i++) {
			t_entrada_tabla_de_paginas * coso = list_get(lista_clock, i);
			log_info(log,
					"XXX PID:%d  PAGINA:%d  MARCO:%d  PRESENCIA:%d  --- (U:%d M:%d) P:%d",
					coso->pid, coso->pagina, coso->marco, coso->presencia,
					coso->uso, coso->modificado, coso->puntero);

		}

		victima->presencia = false;
	}

	else if (string_equals_ignore_case(algoritmo, "MODIFICADO")) {

		log_info(log, "Inicio del algoritmo de reemplazo Clock Modificado.\n");

		pthread_mutex_lock(&semaforo_mutex_tabla_de_paginas);
		t_list * lista_clock_modificado = lista_circular_clock(tabla_de_paginas,
				pid);
		pthread_mutex_unlock(&semaforo_mutex_tabla_de_paginas);

		bool buscar_victima_sin_modificar_uso(void * elemento) {

			t_entrada_tabla_de_paginas * entrada =
					(t_entrada_tabla_de_paginas *) elemento;

			return !entrada->uso && !entrada->modificado;

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
					"No encontro victima buscando (0,0), empieza a buscar (0,1) y a poner el bit de uso en 0 de ser necesario.\n");

			victima = list_find(lista_clock_modificado,
					buscar_victima_y_modificar_uso_2);

			if (victima == NULL) {

				log_info(log,
						"No encontro victima buscando (0,1), vuelve a buscar (0,0).\n");

				victima = list_find(lista_clock_modificado,
						buscar_victima_sin_modificar_uso);

				if (victima == NULL) {

					log_info(log,
							"No encontro victima buscando (0,0), vuelve a buscar (0,1).\n");

					victima = list_find(lista_clock_modificado,
							buscar_victima_y_modificar_uso_2);
				}
			}

		}

		if (victima->modificado) {

			swap_escribir(victima);

			victima->modificado = false;

		}

		entrada_sin_marco->marco = victima->marco;

		entrada_sin_marco->uso = true;

		entrada_sin_marco->presencia = true;

		avanzar_victima(lista_clock_modificado, entrada_sin_marco, victima);

		victima->presencia = false;

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
		t_entrada_tabla_de_paginas * entrada_con_marco_nuevo,
		t_entrada_tabla_de_paginas * victima) {

	t_entrada_tabla_de_paginas * puntero_viejo = head(lista_clock);
	puntero_viejo->puntero = false;

	if (de_una_entrada(lista_clock)) {

		log_info(log, "Solo hay una pagina presente.\n");

		entrada_con_marco_nuevo->puntero = true;

	} else {

		log_info(log, "Tiene mas de una pagina presente.\n");

		int posicion_victima = index_of(lista_clock, victima);

		t_entrada_tabla_de_paginas * nuevo_puntero;

		if (posicion_victima == list_size(lista_clock) - 1) {

			nuevo_puntero = list_get(lista_clock, 0);

		} else {

			nuevo_puntero = list_get(lista_clock, posicion_victima + 1);

		}

		nuevo_puntero->puntero = true;

		log_info(log,
				string_from_format(
						"Se avanza el puntero de la pagina: %d a la: %d\n",
						victima->pagina, nuevo_puntero->pagina));

	}
	victima->puntero = false;

	log_info(log, string_from_format("Pongo el puntero viejo en falso"));
}

bool de_una_entrada(t_list * lista) {

	return list_size(lista) == 1;

}

void escribir_marco(int marco, int offset, int tamanio, void * contenido) {

	int desplazamiento = marco * tamanio_marco;

	memcpy(memoria + desplazamiento + offset, contenido, tamanio);

	log_info(log, "El marco se escribe con exito.\n");

}

bool hay_marcos_disponibles() {

	bool marco_disponible(void * entrada) {

		t_control_marco * marco = (t_control_marco *) entrada;
		return marco->disponible;

	}

	int cantidad_disponible = list_count_satisfying(control_de_marcos,
			marco_disponible);

	return cantidad_disponible != 0;
}

