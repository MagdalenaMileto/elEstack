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


typedef struct __attribute__((packed))t_variable
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



typedef struct __attribute__((packed))t_blocked{
	int sizePcbSerializado;
	t_pcb *pcb;
	char* semaforo;
	char* io;
	int IO_time;
	int semaforoSize;
	int ioSize;
	int sizeTotal;
}t_blocked;




t_blocked *desserializarBLOQUEO(char *serializado);
char *serializarPCB(t_pcb *pcb);
void agregarContexto(t_pcb *pcb,t_contexto *contexto);
t_pcb *desserializarPCB(char *serializado);
void destruirPCB(t_pcb *pcb);
char *serializarBLOQUEO(t_blocked *bloqueo);

#endif /* ESTRUCTURASCONTROL_H_ */




/*


DEVUELVO UN PCB POR FIN DE QUANTUM


	t_pcb *pcbSerializado;
	pcbSerializado = (t_pcb*)serializarPCB(proceso->pcb);
	enviar(sock, 304, pcbSerializado->sizeTotal, pcbSerializado);

	//hacer free de pcbserializado y destruccion de pcb




	
RECIBO UN PCB


		paquete_nuevo = recibir(cpu);
		if (paquete_nuevo->codigo_operacion == 303) {
		
			pcb=desserializarPCB(paquete_nuevo->data);
			printf("CPUMOCK: Recibi pcb %d... ejecutando\n",pcb->pid);




ESTA NO LA PROBE

ENVIO PCB A BLOQUEARSE



typedef struct __attribute__((packed))t_blocked{
	int sizePcbSerializado;
	int sizeTotal;
	char* semaforo;
	char* io;
	int IO_time;
	int semaforoSize;
	int ioSize;
}t_blocked;


t_blocked *bloqueo;

bloqueo=malloc (sizeof(t_blocked));

bloqueo->sizesemaforo=0

bloqueo->io = PUNTERO AL STRING DEL IO
bloqueo->sizeio = sizeof PUNTERO AL STRING DEL IO o strlen (no se como viene la estructura)

ESTO ES AL REVES SI QUIERO BLOQUEAR POR SEMAFORO, pongo en 0 el size del otro bla bla

t_blocked *blockedSerializado;
	blockedSerializado = (t_blocked *)serializarBLOQUEO(bloqueo,pcb);
	enviar(sock, 340, blockedSerializado->sizeTotal, blockedSerializado);


serializarBLOQUEO serializa el pcb, no darselo serializado.











*/
