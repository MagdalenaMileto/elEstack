/*
 * algoritmos.c
 *
 *  Created on: 8/6/2016
 *      Author: utnso
 */

#include "algoritmos.h"

context(algoritmos) {

	describe("funciones en comun para los algoritmos") {

		t_list * lista;

		before {
			lista = list_create();
		}end

		t_entrada_tabla_de_paginas * nueva_entrada(int pid, int pagina) {

			t_entrada_tabla_de_paginas * entrada = crear_nueva_entrada(pid,pagina);
			list_add(lista,entrada);

			return entrada;
		}

		t_entrada_tabla_de_paginas * nueva_entrada_presente(int pid, int pagina) {

			t_entrada_tabla_de_paginas * entrada = nueva_entrada(pid,pagina);
			entrada->presencia = true;
			return entrada;
		}

		int pagina_en_la_posicion(t_list * list,int posicion) {

			t_entrada_tabla_de_paginas * puntero = list_get(list,posicion);

			return puntero->pagina;

		}

		describe("lista_circular_clock") {

			t_entrada_tabla_de_paginas * entrada;

			it("Si no hay entradas presentes es una lista vacia") {

				nueva_entrada(1,0);

				nueva_entrada(1,1);

				nueva_entrada(1,2);

				nueva_entrada(2,0);

				nueva_entrada_presente(2,1);

				t_list * lista_clock = lista_circular_clock(lista,1);

				should_bool(list_is_empty(lista_clock)) be equal to(true);

			}end

			it("solo incluye a las paginas presentes del proceso indicado") {

				nueva_entrada(1,0);

				entrada = nueva_entrada_presente(1,1);
				entrada->puntero = true;
				entrada->marco = 3;

				entrada = nueva_entrada_presente(1,2);
				entrada->puntero = false;
				entrada->marco = 2;

				nueva_entrada(2,0);

				nueva_entrada_presente(2,1);

				t_list * lista_clock = lista_circular_clock(lista,1);

				should_int(list_size(lista_clock)) be equal to(2);

				should_int(pagina_en_la_posicion(lista_clock,0)) be equal to(1);
				should_int(pagina_en_la_posicion(lista_clock,1)) be equal to(2);

			}end

		}end

		describe("avanzar_victima") {

			it("Si hay un solo elemento, la nueva entrada pasa a ser victima") {

				t_entrada_tabla_de_paginas * puntero_nuevo = nueva_entrada(1,0);

				t_entrada_tabla_de_paginas * puntero_viejo = nueva_entrada_presente(1,1);
				puntero_viejo->puntero = true;
				puntero_viejo->marco = 3;

				t_list * lista_clock = lista_circular_clock(lista,1);

				avanzar_victima(lista_clock,puntero_nuevo);

				should_bool(puntero_nuevo->puntero) be equal to(true);
				should_bool(puntero_viejo->puntero) be equal to(false);

			}end

			it("Con mas de un elemento pasa a ser el proximo") {

				t_entrada_tabla_de_paginas * entrada_nueva = nueva_entrada(1,0);

				t_entrada_tabla_de_paginas * puntero_viejo = nueva_entrada_presente(1,1);
				puntero_viejo->puntero = true;
				puntero_viejo->marco = 1;

				t_entrada_tabla_de_paginas * puntero_nuevo = nueva_entrada_presente(1,2);
				puntero_nuevo->puntero = false;
				puntero_nuevo->marco = 2;

				t_list * lista_clock = lista_circular_clock(lista,1);

				avanzar_victima(lista_clock,puntero_viejo,entrada_nueva);

				should_bool(puntero_nuevo->puntero) be equal to(true);
				should_bool(puntero_viejo->puntero) be equal to(false);

			}end

			it("Si es el ultimo pasa a ser el primero") {

				t_entrada_tabla_de_paginas * entrada_nueva = nueva_entrada(1,0);

				t_entrada_tabla_de_paginas * puntero_nuevo = nueva_entrada_presente(1,1);
				puntero_nuevo->puntero = false;
				puntero_nuevo->marco = 1;

				t_entrada_tabla_de_paginas * puntero_viejo = nueva_entrada_presente(1,2);
				puntero_viejo->puntero = true;
				puntero_viejo->marco = 2;

				t_list * lista_clock = lista_circular_clock(lista,1);

				avanzar_victima(lista_clock,entrada_nueva);

				should_bool(puntero_nuevo->puntero) be equal to(true);
				should_bool(puntero_viejo->puntero) be equal to(false);

			}end

		}end

	}end

}
