#include "leer.h"

void * leer_una_pagina(int numero_pagina, int offset, int tamanio) {

	t_entrada_tabla_de_paginas * pagina_encontrada = buscar_tlb(numero_pagina);

	if (!pagina_encontrada->presencia) {

		char * contenido_faltante = swap_leer(proceso_actual, numero_pagina);

		marco_nuevo(pagina_encontrada);

		escribir_marco(pagina_encontrada->marco, 0, tamanio_marco,
				contenido_faltante);

	}

	pagina_encontrada->uso = true;

	void * contenido = malloc(tamanio);

	int desplazamiento = pagina_encontrada->marco * tamanio_marco + offset;

	memcpy(contenido, memoria + desplazamiento, tamanio);

	return contenido;

}

