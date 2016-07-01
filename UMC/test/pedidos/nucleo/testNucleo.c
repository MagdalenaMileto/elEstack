/*
 * testNucleo.c
 *
 *  Created on: 19/5/2016
 *      Author: utnso
 */

#include "testNucleo.h"

context( nucleo) {

	describe("Funciones Nucleo") {

		before {
			log = log_create(ARCHIVOLOG, "UMC", 0, LOG_LEVEL_INFO);
		}end

		describe("Inicializar") {

			before {
				tabla_de_paginas = list_create();
			}end

			it(
					"se crean exitosamente las paginas de un proceso") {

				inicializar_programa(1, 4);

				should_int(
						list_size(tabla_de_paginas))be equal to(4);

			}end

		}end

		describe("Paginas correctas") {

			before {
				tabla_de_paginas = list_create();
			}end

			it(
					"Las páginas se crean en un orden creciente y empezando desde 0") {

				inicializar_programa(1, 4);

				t_entrada_tabla_de_paginas * entrada1 = list_get(tabla_de_paginas,0);
				t_entrada_tabla_de_paginas * entrada2 = list_get(tabla_de_paginas,1);
				t_entrada_tabla_de_paginas * entrada3 = list_get(tabla_de_paginas,2);
				t_entrada_tabla_de_paginas * entrada4 = list_get(tabla_de_paginas,3);

				should_int(
						entrada1->pagina)be equal to(0);should_int(entrada2->pagina)be equal to(1);should_int(
						entrada3->pagina)be equal to(2);should_int(entrada4->pagina)be equal to(3);

			}end

		}end

		describe("Finalizar") {

			before {
				tabla_de_paginas = list_create();
				tlb = list_create();

				cantidad_marcos = 2;
				inicializar_marcos();
			}end

			void mock_finalizar_swap(int pid) {
				eliminar_proceso_tlb(pid);
				eliminar_proceso_tabla_de_paginas(pid);
			}

			it("Finalizar elimina la entrada de la tlb y la tabla de paginas, poniendo su marco como libre") {

				inicializar_programa(0, 2);

				t_entrada_tabla_de_paginas * entrada1 = list_get(tabla_de_paginas,1);

				entrada1->presencia = true;
				entrada1->marco = 1;

				list_add(tlb,entrada1);

				t_control_marco * marco1 = list_get(control_de_marcos,1);
				marco1->disponible = false;

				inicializar_programa(1, 2);

				mock_finalizar_swap(0);

				should_bool(marco1->disponible) be equal to(true);

				should_bool(list_size(tlb)) be equal to(0);
				should_bool(list_size(tabla_de_paginas)) be equal to(2);
			}end

		}end

	}end

}

