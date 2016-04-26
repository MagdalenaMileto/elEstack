/*
 * estructurasControl.h
 *
 *	Interfaces para los distintos tipos de estructuras de control
 *	requeridos por los distintos modulos
 *
 *  Created on: 26/4/2016
 *      Author: Franco Albornoz
 */
#ifndef PARSER_H_
#include <parser/parser.h>
#endif

#ifndef ESTRUCTURASCONTROL_H_
#define ESTRUCTURASCONTROL_H_

// Estructura para el PCB
// Faltan el indice de codigo y el indice de etiquetas
typedef struct {
	int pid;
	unsigned int pc;
	unsigned int codePages;
	t_puntero sc;
}t_pcb;

#endif /* ESTRUCTURASCONTROL_H_ */
