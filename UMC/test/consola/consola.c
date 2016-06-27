/*
 * consola.c
 *
 *  Created on: 17/6/2016
 *      Author: utnso
 */

#include "consola.h"

context(consola) {

	describe("Dump") {

		before {
			tabla_de_paginas = list_create();
			remove("./memory.dump");
		}end

		describe("dump total") {

			void escribir(int pid, int pagina, int tamanio, void * contenido) {

				bool pid_y_pagina(void * elemento) {
					t_entrada_tabla_de_paginas * entrada =
					(t_entrada_tabla_de_paginas *) elemento;

					return entrada->pid == pid && entrada->pagina == pagina;
				}

				t_entrada_tabla_de_paginas * entrada_a_escribir =
				list_filter(tabla_de_paginas,pid_y_pagina);

				marco_nuevo(entrada_a_escribir);

				memcpy(memoria + entrada_a_escribir->marco * tamanio_marco,contenido,tamanio);

				entrada_a_escribir-> modificado = true;
			}

			it("Escribe el archivo de dump" ) {

				tamanio_marco = 5;
				cantidad_marcos = 4;
				solicitar_bloque_memoria();
				inicializar_marcos();

				inicializar_programa(0,2);
				inicializar_programa(1,1);
				inicializar_programa(2,1);

				escribir(0,0,5,"hola");
				escribir(2,0,7,"ke ase");

				dump_total();

			}end

		}end

	}end
}

