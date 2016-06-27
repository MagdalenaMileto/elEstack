#include "finalizar.h"

void finalizar_proceso(int pid) {

	log_info(log, "El Nucleo envia una peticion para finalizar el proceso %id",
			pid);

	eliminar_proceso_tabla_de_paginas(pid);
	swap_finalizar_proceso(pid);
}

