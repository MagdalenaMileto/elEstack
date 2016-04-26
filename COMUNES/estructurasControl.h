/*
 * estructurasControl.h
 *
 *  Created on: 26/4/2016
 *      Author: Franco Albornoz
 */
#ifndef PARSER_H_
#include <parser/parser.h>
#endif

#ifndef ESTRUCTURASCONTROL_H_
#define ESTRUCTURASCONTROL_H_

typedef struct {
	int pid;
	unsigned int pc;
	t_puntero sc;
}t_pcb;

#endif /* ESTRUCTURASCONTROL_H_ */
