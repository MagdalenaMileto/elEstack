/*
 * clock-m.c
 *
 *  Created on: 30/6/2016
 *      Author: utnso
 */

#include "clock-m.h"

context(algoritmo_clock_modificado) {

	describe("Algoritmo clock modificado") {

		before {

			log = log_create(ARCHIVOLOG, "UMC", 0, LOG_LEVEL_INFO);
			tabla_de_paginas = list_create();

		}end

		void nueva_entrada(int pid, int pagina) {

			t_entrada_tabla_de_paginas * entrada = crear_nueva_entrada(pid, pagina);
			list_add(tabla_de_paginas, entrada);

		}

		it("Reemplaza acorde a lo esperando") {

			nueva_entrada(2,0);

			t_entrada_tabla_de_paginas * entrada_0 = crear_nueva_entrada(1,0);
			entrada_0->marco = 0;
			entrada_0->presencia = true;
			entrada_0->puntero = false;

		}end

	}end

}
