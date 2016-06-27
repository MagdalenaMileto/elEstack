/*
 * testCommmons.c
 *
 *  Created on: 19/5/2016
 *      Author: utnso
 */

#include "testCommons.h"

context(commons) {

	describe("Funciones de listas") {

		t_list * lista;

		before {
			log = log_create(ARCHIVOLOG, "UMC", 0, LOG_LEVEL_INFO);
			lista = list_create();
		}end

		describe("Remover y destruir") {

			bool pid_es_dos(void *elemento) {

				t_entrada_tabla_de_paginas * entrada = (t_entrada_tabla_de_paginas *) elemento;

				return entrada->pid == 2;
			}

			it("Elimina y destruye todos los elementos que matcheen") {

				list_add(lista,crear_nueva_entrada(1,1));
				list_add(lista,crear_nueva_entrada(1,2));
				list_add(lista,crear_nueva_entrada(2,1));
				list_add(lista,crear_nueva_entrada(1,3));
				list_add(lista,crear_nueva_entrada(2,2));

				remove_and_destroy_all_such_that(lista,pid_es_dos,free);

				should_int(list_size(lista)) be equal to(3);

			}end

			it("Si nadie matchea no destruye nada") {

				list_add(lista,crear_nueva_entrada(1,1));
				list_add(lista,crear_nueva_entrada(1,2));
				list_add(lista,crear_nueva_entrada(2,1));
				list_add(lista,crear_nueva_entrada(1,3));
				list_add(lista,crear_nueva_entrada(2,2));

				remove_and_destroy_all_such_that(lista,pid_es_dos,free);

				should_int(list_size(lista)) be equal to(3);

			}end

		}end

		void agregar_numero(int valor) {

			int * puntero = malloc(sizeof(int));

			*puntero = valor;
			list_add(lista,puntero);

		}

		int * agregar_numero_y_puntero(int valor) {

			int * puntero = malloc(sizeof(int));

			*puntero = valor;
			list_add(lista,puntero);
			return puntero;
		}

		describe("index_of") {

			int * elemento_elegido;

			it("Devuelve el indice si es el primer elemento") {

				elemento_elegido = agregar_numero_y_puntero(-10);
				agregar_numero(3);
				agregar_numero(6);

				should_int(index_of(lista,elemento_elegido)) be equal to(0);

			}end

			it("Devuelve el indice si es el ultimo elemento") {

				agregar_numero(3);
				agregar_numero(6);
				elemento_elegido = agregar_numero_y_puntero(-10);

				should_int(index_of(lista,elemento_elegido)) be equal to(2);

			}end

			it("Devuelve el indice si es un elemento intermedio") {

				agregar_numero(3);
				elemento_elegido = agregar_numero_y_puntero(-10);
				agregar_numero(6);

				should_int(index_of(lista,elemento_elegido)) be equal to(1);

			}end

			it("Devuelve -1 si es el elemento no existe") {

				agregar_numero(3);
				agregar_numero(-10);
				agregar_numero(6);

				int * elemento_no_agregado = malloc(sizeof(4));

				should_int(index_of(lista,elemento_no_agregado)) be equal to(-1);

			}end

		}end

		describe("list_drop") {

			before {
				agregar_numero(3);
				agregar_numero(6);
				agregar_numero(9);
			}end

			it("una cantidad negativa no dropea nada") {

				t_list * resultado = list_drop(lista,-1);

				should_int(list_size(resultado)) be equal to(3);

			}end

			it("con valores intermedios") {

				t_list * resultado = list_drop(lista,0);

				should_int(list_size(resultado)) be equal to(3);

				resultado = list_drop(lista,1);

				should_int(list_size(resultado)) be equal to(2);

				resultado = list_drop(lista,2);

				should_int(list_size(resultado)) be equal to(1);

			}end

			it("con una cantidad igual al largo es una lista vacia") {

				t_list * resultado = list_drop(lista,3);

				should_int(list_size(resultado)) be equal to(0);

			}end

			it("una cantidad mayor al largo sigue siendo una lista vacia") {

				t_list * resultado = list_drop(lista,100);

				should_int(list_size(resultado)) be equal to(0);

			}end

		}end

		describe("circular_list_starting_with") {
			int * elemento_elegido;

			int valor_en_la_posicion(t_list * list,int posicion) {

				int * puntero = list_get(list,posicion);

				return *puntero;

			}

			it("eligiendo el primer elemento no altera nada") {

				elemento_elegido = agregar_numero_y_puntero(3);
				agregar_numero(6);
				agregar_numero(9);

				t_list * resultado = circular_list_starting_with(lista,elemento_elegido);

				should_int(list_size(resultado)) be equal to(3);
				should_int(list_size(lista)) be equal to(3);

				should_int(valor_en_la_posicion(resultado,0)) be equal to(3);
				should_int(valor_en_la_posicion(resultado,1)) be equal to(6);
				should_int(valor_en_la_posicion(resultado,2)) be equal to(9);

				should_int(valor_en_la_posicion(lista,0)) be equal to(3);
				should_int(valor_en_la_posicion(lista,1)) be equal to(6);
				should_int(valor_en_la_posicion(lista,2)) be equal to(9);

			}end

			it("un valor intermedio reordena la lista adecuadamente") {

				agregar_numero(6);
				elemento_elegido = agregar_numero_y_puntero(3);
				agregar_numero(9);

				t_list * resultado = circular_list_starting_with(lista,elemento_elegido);

				should_int(list_size(resultado)) be equal to(3);
				should_int(list_size(lista)) be equal to(3);

				should_int(valor_en_la_posicion(resultado,0)) be equal to(3);
				should_int(valor_en_la_posicion(resultado,1)) be equal to(9);
				should_int(valor_en_la_posicion(resultado,2)) be equal to(6);

				should_int(valor_en_la_posicion(lista,0)) be equal to(6);
				should_int(valor_en_la_posicion(lista,1)) be equal to(3);
				should_int(valor_en_la_posicion(lista,2)) be equal to(9);

			}end

			it("el ultimo valor solo coloca al elegido adelante") {

				agregar_numero(6);
				agregar_numero(9);
				elemento_elegido = agregar_numero_y_puntero(3);

				t_list * resultado = circular_list_starting_with(lista,elemento_elegido);

				should_int(list_size(resultado)) be equal to(3);
				should_int(list_size(lista)) be equal to(3);

				should_int(valor_en_la_posicion(resultado,0)) be equal to(3);
				should_int(valor_en_la_posicion(resultado,1)) be equal to(6);
				should_int(valor_en_la_posicion(resultado,2)) be equal to(9);

				should_int(valor_en_la_posicion(lista,0)) be equal to(6);
				should_int(valor_en_la_posicion(lista,1)) be equal to(9);
				should_int(valor_en_la_posicion(lista,2)) be equal to(3);

			}end

		}end

	}end

}

