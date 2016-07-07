#include "leer.h"

void * leer_una_pagina(int pid, int numero_pagina, int offset, int tamanio) {

	log_info(log,
			"Llega una peticion de lectura por parte de una CPUs, cuyo proceso es %d, la pagina deseada es %d con el offset %d.\n",
			pid, numero_pagina, offset);

	t_entrada_tabla_de_paginas * pagina_encontrada;

	if (tlb_habilitada()) {

		pagina_encontrada = buscar_tlb(pid, numero_pagina);

	} else {

		log_info(log,
				"La TLB esta apagada, se busca en la tabla de paginas.\n");

		pagina_encontrada = buscar_pagina_tabla_de_paginas(pid, numero_pagina);
	}

	if (!pagina_encontrada->presencia) {

		pthread_mutex_lock(&semaforo_mutex_marcos);

		if (no_tiene_ni_hay_marcos(pid)) {

			pthread_mutex_unlock(&semaforo_mutex_marcos);
			log_info(log,
					"Se finaliza el proceso %d por no haber marcos libres ni el tener uno.\n",
					pid);

			return string_from_format("No hay lugar disponible!");

		}

		char * contenido_faltante = swap_leer(pid, numero_pagina);

		marco_nuevo(pagina_encontrada);

		escribir_marco(pagina_encontrada->marco, 0, tamanio_marco,
				contenido_faltante);

		pagina_encontrada->presencia = true;

	}

	pagina_encontrada->uso = true;

	void * contenido = malloc(tamanio);

	int desplazamiento = pagina_encontrada->marco * tamanio_marco + offset;

	memcpy(contenido, memoria + desplazamiento, tamanio);

	log_info(log, "Se leyo con exito la pagina %d.\n", numero_pagina);

	return contenido;

}

bool no_tiene_ni_hay_marcos(int pid) {

	bool coincide_pid_y_esta_presente(void * elemento) {

		t_entrada_tabla_de_paginas * entrada =
				(t_entrada_tabla_de_paginas *) elemento;

		return entrada->pid == pid && entrada->presencia;
	}

	pthread_mutex_lock(&semaforo_mutex_tabla_de_paginas);
	bool tiene_alguno = list_any_satisfy(tabla_de_paginas,
			coincide_pid_y_esta_presente);
	pthread_mutex_unlock(&semaforo_mutex_tabla_de_paginas);

	return !tiene_alguno && !hay_marcos_disponibles();

}

