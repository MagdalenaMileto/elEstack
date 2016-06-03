#include "leer.h"

void * leer_una_pagina(int numero_pagina, int offset, int tamanio) {

	//TODO: Buscar en la TLB
	//t_entrada_tlb * pagina_encontrada_tlb = buscar_en_tlb(numero_pagina);
	//if (!pagina_encontrada_tlb->presencia){

	t_entrada_tabla_de_paginas * pagina_encontrada =
			buscar_pagina_tabla_de_paginas(numero_pagina);

	if (!pagina_encontrada->presencia) {

		char * contenido_faltante = swap_leer(proceso_actual, numero_pagina);

		//TODO pagina_encontrada->marco = funcion_magica()???

		int desplazamiento = pagina_encontrada->marco * tamanio_marco;
		memcpy(memoria + desplazamiento, contenido_faltante, tamanio_marco);

		pagina_encontrada->presencia = true;

	}

	void * contenido = malloc(tamanio);

	memcpy(contenido, pagina_encontrada + offset, tamanio);

	return contenido;
}

void * buscar_pagina_tabla_de_paginas( numero_pagina) {

	bool filtrar_por_proceso_y_pagina(void * elemento_de_la_lista) {

		t_entrada_tabla_de_paginas * entrada = elemento_de_la_lista;

		return entrada->pagina == numero_pagina
				&& entrada->pid == proceso_actual;

	}

	t_list * resultado = list_filter(tabla_de_paginas,
			filtrar_por_proceso_y_pagina);

	return list_get(resultado, 0);

}
