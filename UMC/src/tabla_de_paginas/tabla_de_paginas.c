#include "tabla_de_paginas.h"

t_entrada_tabla_de_paginas * crear_nueva_entrada(int pid, int pagina) {

	t_entrada_tabla_de_paginas * nueva_entrada = malloc(
			sizeof(t_entrada_tabla_de_paginas));

	nueva_entrada->pid = pid;
	nueva_entrada->pagina = pagina;
	nueva_entrada->marco = 999;
	nueva_entrada->presencia = false;
	nueva_entrada->modificado = false;

	return nueva_entrada;
}

void eliminar_proceso_tabla_de_paginas(int pid) {

	bool lambda_coincide_pid(void * elemento) {

		t_entrada_tabla_de_paginas * entrada =
				(t_entrada_tabla_de_paginas*) elemento;

		return entrada->pid == pid;

	}

	remove_and_destroy_all_such_that(tabla_de_paginas, lambda_coincide_pid,
			free);
}

