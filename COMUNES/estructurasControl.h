/*
 *  estructurasControl.h
 *
 *	Interfaces para los distintos tipos de estructuras de control
 *	requeridos por los distintos modulos
 *
 *  Created on: 26/4/2016
 *      Author: Franco Albornoz
 */

#include <parser/parser.h>
#include <stdint.h>

#ifndef ESTRUCTURASCONTROL_H_
#define ESTRUCTURASCONTROL_H_

// Estructura para identificar variables con una etiqueta y una posicion en el stack
typedef struct variables
{
	char etiqueta;
	t_puntero pos;
} variables;

// Estructura de contexto para el stack
typedef struct context
{
	int pos;
	t_puntero *args;
	variables *vars;
	int retPos;
	t_puntero retVar;
}context;

// Estructura para el PCB
typedef struct __attribute__((packed))t_pcb{
	int pid;
	unsigned int pc;
	unsigned int codePages;
	u_int16_t *codeIndex;
	void *labelIndex;	// Hace falta definir bien esto
	t_puntero sc;
}t_pcb;

#endif /* ESTRUCTURASCONTROL_H_ */
