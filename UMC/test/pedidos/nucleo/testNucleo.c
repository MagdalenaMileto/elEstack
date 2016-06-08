/*
 * testNucleo.c
 *
 *  Created on: 19/5/2016
 *      Author: utnso
 */

#include "testNucleo.h"

context( nucleo) {

	describe("Funciones Nucleo")
	{

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

	}end
}

