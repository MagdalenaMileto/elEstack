#include "escribir.h"

void escribir_una_pagina(int pid, int numero_pagina, int offset, int tamanio,
		void * buffer) {

	log_info(log,
			"Llega una peticion de escritura por parte de una de las CPUs, donde la pagina solicitada a escribir es la %d, el proceso es %d y su offset %d.\n",
			numero_pagina, pid, offset);

	t_entrada_tabla_de_paginas * pagina_encontrada;

	if (tlb_habilitada()) {

		pagina_encontrada = buscar_tlb(pid, numero_pagina);

	} else {

		log_info(log, "La TLB esta apagada, se busca en la tabla de paginas\n");
		pagina_encontrada = buscar_pagina_tabla_de_paginas(pid, numero_pagina);
	}

	if (!pagina_encontrada->presencia) {

		void * contenido_faltante = swap_leer(pid, numero_pagina);

		marco_nuevo(pagina_encontrada);

		escribir_marco(pagina_encontrada->marco, 0, tamanio_marco,
				contenido_faltante);

		pagina_encontrada->presencia = true;

	}

	escribir_marco(pagina_encontrada->marco, offset, tamanio, buffer);

	pagina_encontrada->modificado = true;
	pagina_encontrada->uso = true;

	log_info(log, "Se escribio con exito la pagina %d.\n", numero_pagina);

}

