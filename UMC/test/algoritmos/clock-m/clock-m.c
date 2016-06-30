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
			algoritmo = "CLOCK-M";

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

			t_entrada_tabla_de_paginas * entrada_1 = crear_nueva_entrada(1,1);
			entrada_1->marco = 1;
			entrada_1->presencia = true;
			entrada_1->puntero = true;

			t_entrada_tabla_de_paginas * entrada_2 = crear_nueva_entrada(1,2);
			entrada_2->uso = false;

			list_add(tabla_de_paginas,entrada_0);
			list_add(tabla_de_paginas,entrada_1);
			list_add(tabla_de_paginas,entrada_2);

			algoritmo_remplazo(entrada_2,1);

		}end

	}end

}
