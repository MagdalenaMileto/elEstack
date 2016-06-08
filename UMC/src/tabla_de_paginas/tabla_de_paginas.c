#include "tabla_de_paginas.h"

t_entrada_tabla_de_paginas * buscar_pagina_tabla_de_paginas(int numero_pagina) {

	bool filtrar_por_proceso_y_pagina(void * elemento_de_la_lista) {

		t_entrada_tabla_de_paginas * entrada = elemento_de_la_lista;

		return entrada->pagina == numero_pagina
				&& entrada->pid == proceso_actual;
	}

	t_entrada_tabla_de_paginas * pagina_encontrada = list_find(tabla_de_paginas,
			filtrar_por_proceso_y_pagina);

	return pagina_encontrada;

}

t_entrada_tabla_de_paginas * crear_nueva_entrada(int pid, int pagina) {

	t_entrada_tabla_de_paginas * nueva_entrada = malloc(
			sizeof(t_entrada_tabla_de_paginas));

	nueva_entrada->pid = pid;
	nueva_entrada->pagina = pagina;
	nueva_entrada->marco = -1;
	nueva_entrada->presencia = false;
	nueva_entrada->modificado = false;
	nueva_entrada->puntero = false;
	nueva_entrada->uso = true;

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

