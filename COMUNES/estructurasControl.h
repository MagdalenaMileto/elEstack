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


typedef struct __attribute__((packed))t_direccion{
	int pagina;
	int offset;
	int size;
}t_direccion;


typedef struct __attribute__((packed))variable
{
	char etiqueta;
	t_direccion direccion;
} t_variable;



// Estructura de contexto para el stack
typedef struct __attribute__((packed))t_contexto
{
	int pos;
	t_direccion **args;
	t_variable **vars;
	int retPos;
	t_direccion retVar;
	int sizeArgs;
	int sizeVars;
}t_contexto;

// Estructura para el PCB
typedef struct __attribute__((packed))t_pcb{
	int pid;
	unsigned int pc;
	unsigned int paginasDeCodigo;
	int *indiceDeCodigo; 
	char *indiceDeEtiquetas;	
	t_contexto **contextoActual; //Aca podria almanecar la lista de contextos en el nucleo y solo pasarle el contexto que necesito dentro del pcb?
	int sizeContextoActual;
	int sizeIndiceDeEtiquetas;
	int sizeIndiceDeCodigo;
	int sizeTotal;
}t_pcb;

#endif /* ESTRUCTURASCONTROL_H_ */
