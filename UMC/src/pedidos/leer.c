#include "leer.h"

void * leer_una_pagina(int numero_pagina, int offset, int tamanio) {

	log_info(log,
			"Llega una peticion de lectura por parte de una CPUs, cuyo proceso es %d, la pagina deseada es %d con el offset %d",
			proceso_actual, numero_pagina, offset);

	t_entrada_tabla_de_paginas * pagina_encontrada = buscar_tlb(numero_pagina);

	if (!pagina_encontrada->presencia) {

		if (no_tiene_ni_hay_marcos(proceso_actual)) {

			log_info(log,
					"Se finaliza el proceso %d por no haber marcos libres ni el tener uno",
					proceso_actual);

			return string_from_format("No hay lugar disponible!");;

		}

		char * contenido_faltante = swap_leer(proceso_actual, numero_pagina);
		marco_nuevo(pagina_encontrada);

		escribir_marco(pagina_encontrada->marco, 0, tamanio_marco,
				contenido_faltante);

		pagina_encontrada->presencia = true;

	}

	pagina_encontrada->uso = true;

	void * contenido = malloc(tamanio);

	int desplazamiento = pagina_encontrada->marco * tamanio_marco + offset;

	memcpy(contenido, memoria + desplazamiento, tamanio);

	log_info(log, "Se leyo con exito la pagina %d.", numero_pagina);

	return contenido;

}

bool no_tiene_ni_hay_marcos(int pid) {
	printf("HOLAAAA\n");
	bool coincide_pid_y_esta_presente(void * elemento) {

		t_entrada_tabla_de_paginas * entrada =
				(t_entrada_tabla_de_paginas *) elemento;

		return entrada->pid == pid && entrada->presencia;
	}

	bool tiene_alguno = list_any_satisfy(tabla_de_paginas,
			coincide_pid_y_esta_presente);
	printf("CACACAAA NENE SE HACE CACA %d %d",tiene_alguno,hay_marcos_disponibles());
	return !tiene_alguno && !hay_marcos_disponibles();

}

