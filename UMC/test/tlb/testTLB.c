/*
 * tlb.c
 *
 *  Created on: 5/6/2016
 *      Author: utnso
 */

#include "testTLB.h"

context(tlb) {
	describe("Funciones de la TLB") {

		before {
			log = log_create(ARCHIVOLOG, "UMC", 0, LOG_LEVEL_INFO);
		}end

		after {
			log_destroy(log);
		}end

		describe("Agregar Entradas") {

			before {
				tlb = list_create();
				cantidad_marcos = 10;
				entradas_TLB = 15;
				cantidad_maxima_marcos = 4;
				numero_operacion_actual = 0;
			}end

			after {
				list_destroy(tlb);
			}end

			t_entrada_tabla_de_paginas * entrada_proceso;

			it(
					"La función me dice si se agregan correctamente las entradas") {

				//En realidad son entradas a la TLB. Si, la expresividad se fue a la mierda.

				agregar_entrada_tlb(crear_nueva_entrada(1,0));
				agregar_entrada_tlb(crear_nueva_entrada(1,1));
				agregar_entrada_tlb(crear_nueva_entrada(1,2));
				agregar_entrada_tlb(crear_nueva_entrada(2,0));

				should_int(
						list_size(tlb))be equal to(4);
			}end

			it("Agregar una entrada incrementa el contador de operacion") {

				entrada_proceso = crear_nueva_entrada(1,0);

				agregar_entrada_tlb(entrada_proceso);

				should_int(entrada_proceso->ultima_vez_usado)be equal to(0);

				entrada_proceso = crear_nueva_entrada(1,1);

				agregar_entrada_tlb(entrada_proceso);

				should_int(entrada_proceso->ultima_vez_usado)be equal to(1);

			}end

			it("Si no se le pueden asignar mas marcos se swappea el mas viejo") {

				entradas_TLB = 3;

				agregar_entrada_tlb(crear_nueva_entrada(1,0));
				agregar_entrada_tlb(crear_nueva_entrada(1,1));
				agregar_entrada_tlb(crear_nueva_entrada(1,2));

				agregar_entrada_tlb(crear_nueva_entrada(1,3));

				entrada_proceso = head(tlb);

				should_int(entrada_proceso->pagina) be equal to(1);
				should_int(list_size(tlb)) be equal to(3);

			}end
		}end

		describe("Eliminar Entradas") {

			before {
				tlb = list_create();
				entradas_TLB = 15;
				numero_operacion_actual = 0;
			}end

			after {
				list_destroy(tlb);
			}end

			t_entrada_tabla_de_paginas * entrada_proceso;

			it(
					"Se elimina solamente la entrada de la tlb y no se libera") {

				//En realidad son entradas a la TLB. Si, la expresividad se fue a la mierda.

				entrada_proceso = crear_nueva_entrada(2,0);

				agregar_entrada_tlb(crear_nueva_entrada(1,0));
				agregar_entrada_tlb(crear_nueva_entrada(1,1));
				agregar_entrada_tlb(crear_nueva_entrada(1,2));
				agregar_entrada_tlb(entrada_proceso);

				should_int(
						list_size(tlb))be equal to(4);

				eliminar_proceso_tlb(2);

				should_ptr(entrada_proceso) not be null;

				should_int(
						list_size(tlb))be equal to(3);

			}end
		}end
	}end

}
