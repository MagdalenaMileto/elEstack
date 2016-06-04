#include "./inicializar.h"

bool puede_iniciar_proceso(int pid, int cantidad_paginas, char * codigo) {

	return swap_inicializar_proceso(pid,cantidad_paginas, codigo);

}

void inicializar_programa(int pid, int paginas_requeridas) {

	int var;

	for (var = 0; var < paginas_requeridas; ++var) {

		list_add(tabla_de_paginas, crear_nueva_entrada(pid, var));
	}

}
