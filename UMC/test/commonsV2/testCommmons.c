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
			lista = list_create();
		}end


		describe("Remover y destruir") {

			bool pid_es_dos(void *elemento) {

				t_entrada_tabla_de_paginas * entrada = (t_entrada_tabla_de_paginas *) elemento;

				return entrada->pid ==2;
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

	}end

}

