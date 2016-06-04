#include "finalizar.h"


void finalizar_proceso(int pid) {

	eliminar_proceso_tabla_de_paginas(pid);
	swap_finalizar_proceso(pid);
}


