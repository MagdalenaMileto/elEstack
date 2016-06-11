#include "escribir.h"

void escribir_una_pagina(int numero_pagina, int offset, int tamanio,
		void * buffer) {

	t_entrada_tabla_de_paginas * pagina_encontrada = buscar_tlb(numero_pagina);

	if (!pagina_encontrada->presencia) {

		void * contenido_faltante = swap_leer(proceso_actual, numero_pagina);

		marco_nuevo(pagina_encontrada);

		escribir_marco(pagina_encontrada->marco, 0, tamanio_marco,
				contenido_faltante);

	}

	escribir_marco(pagina_encontrada->marco, offset, tamanio, buffer);

	pagina_encontrada->modificado = true;
	pagina_encontrada->uso = true;

}

