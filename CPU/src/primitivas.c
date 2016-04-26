/*
 * primitivas.c
 *
 *  Created on: 26/4/2016
 *      Author: Franco Albornoz
 */

#include "primitivas.h"
#include <stdlib.h>

t_puntero definirVariable(t_nombre_variable identificador_variable)
{
	printf("Soy la funcion definirVariable\n");
	return 0;
}

t_puntero obtenerPosicionVariable (t_nombre_variable identificador_variable)
{
	printf("Soy la funcion obtenerPosicionVariable\n");
	return 0;
}

t_valor_variable dereferenciar(t_puntero direccion_variable)
{
	printf("Soy la funcion dereferenciar\n");
	return 0;
}

void asignar(t_puntero direccion_variable,t_valor_variable valor)
{
	printf("Soy la funcion asignar\n");
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable)
{
	printf("Soy la funcion obtenerValorCompartida\n");
	return 0;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable,t_valor_variable valor)
{
	printf("Soy la funcion asignarValorCompartida\n");
	return 0;
}

t_puntero_instruccion irAlLabel(t_nombre_etiqueta etiqueta)
{
	printf("Soy la funcion irAlLabel\n");
	return 0;
}

t_puntero_instruccion llamarFuncion(t_nombre_etiqueta etiqueta,t_puntero donde_retornar,t_puntero_instruccion linea_en_ejecucion)
{
	printf("Soy la funcion llamarFuncion\n");
	return 0;
}

t_puntero_instruccion retornar(t_valor_variable retorno)
{
	printf("Soy la funcion retornar\n");
	return 0;
}

int imprimir(t_valor_variable valor_mostrar)
{
	printf("Soy la funcion imprimir\n");
	return 0;
}

int imprimirTexto(char*texto)
{
	printf("Soy la funcion imprimirTexto\n");
	return 0;
}

int entradaSalida(t_nombre_dispositivo dispositivo,int tiempo)
{
	printf("Soy la funcion entradaSalida\n");
	return 0;
}

int wait(t_nombre_semaforo identificador_semaforo)
{
	printf("Soy la funcion wait\n");
	return 0;
}

int signal(t_nombre_semaforo identificador_semaforo)
{
	printf("Soy la funcion signal\n");
	return 0;
}
