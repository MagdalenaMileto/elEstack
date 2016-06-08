/*
 * marcos.c
 *
 *  Created on: 8/6/2016
 *      Author: utnso
 */

#include "marcos.h"

context(asignacion_de_marcos) {

	describe("Asignacion de marcos") {

		before {
			tabla_de_paginas = list_create();
		}end

		it("Si no se llegó al nivel máximo de marcos se asigna uno libre") {

			cantidad_marcos = 2;

			inicializar_marcos();

			t_entrada_tabla_de_paginas * entrada_0 = crear_nueva_entrada(1,0);

			list_add(tabla_de_paginas,entrada_0);

			marco_nuevo(entrada_0);

			should_int(entrada_0->marco) be equal to(0);
			should_bool(entrada_0->uso) be equal to(true);
			should_bool(entrada_0->presencia) be equal to(true);
			should_bool(entrada_0->puntero) be equal to(true);

			t_entrada_tabla_de_paginas * entrada_1 = crear_nueva_entrada(1,1);

			list_add(tabla_de_paginas,entrada_1);

			marco_nuevo(entrada_1);

			should_int(entrada_1->marco) be equal to(1);
			should_bool(entrada_1->uso) be equal to(true);
			should_bool(entrada_1->presencia) be equal to(true);
			should_bool(entrada_1->puntero) be equal to(false);

		}end

	}end

}
