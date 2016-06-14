/*
 * primitivas.c
 *
 *  Created on: 26/4/2016
 *      Author: Franco Albornoz
 */

#include "primitivas.h"
#include <stdlib.h>
#include "../../COMUNES/estructurasControl.h"
#include "../../COMUNES/estructurasControl.c"
#include "funcionesCPU.h"



t_puntero definirVariable(t_nombre_variable identificador_variable)
{
	t_direccion direccion_variable;
	int posicionStack = pcb->sizeContextoActual-1;

	if(pcb->sizeContextoActual==0){
		pcb->contextoActual[0]->pos=0;
		pcb->contextoActual[0]->vars[0]->etiqueta=identificador_variable;
		pcb->contextoActual[0]->vars[0]->direccion = armarDireccionPrimeraPagina();
		direccion_variable = pcb->contextoActual[0]->vars[0]->direccion;

	}else{

			if(pcb->contextoActual[posicionStack]->sizeVars == 0){
			//	pcb->contextoActual[posicionStack]->pos = posicionStack; esto ya va a estar definido cuando se llama a function
				pcb->contextoActual[posicionStack]->vars[0]->etiqueta=identificador_variable;
				pcb->contextoActual[posicionStack]->vars[0]->direccion= armarDirecccionDeFuncion();
				direccion_variable = pcb->contextoActual[posicionStack]->vars[0]->direccion;

				}else{

					int posicionVars = pcb->contextoActual->sizeVars;
					pcb->contextoActual[posicionStack]->pos = posicionStack;
					pcb->contextoActual[posicionStack]->vars[posicionVars]->etiqueta=identificador_variable;
					pcb->contextoActual[posicionStack]->vars[posicionVars]->direccion = armarProximaDireccion();
					direccion_variable = pcb->contextoActual[posicionStack]->vars[posicionVars]->direccion;
		 	  }
		 }


	enviar(umc, 404, sizeof(t_direccion), direccion_variable);
	return direccion_variable; //retornar t_puntero?


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

void llamarFuncion(t_nombre_etiqueta etiqueta, t_puntero donde_retornar)
{
		printf("Soy la funcion llamarConRetorno\n");
		printf("Guardo contexto que es la etiqueta y retornar el valor\n");
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

t_direccion armarDireccionPrimeraPagina(){
	t_direccion direccion;
	direccion.offset=0;
	direccion.size=4;
	direccion.pagina=primeraPagina();

	return direccion;
}

t_direccion armarProximaDireccion(){
	int ultimaPosicionStack = pcb->sizeContextoActual-1;
	int posicionUltimaVariable = pcb->contextoActual[ultimaPosicionStack]->sizeVars-1;

	return proximaDireccion(ultimaPosicionStack,posicionUltimaVariable);
}

int primeraPagina(){
	return ((pcb->indiceDeCodigo[pcb->sizeIndiceDeCodigo-2])+pcb->indiceDeCodigo[pcb->sizeIndiceDeCodigo-1]/ tamanioPag)+1;
}

t_direccion armarDireccionDeFuncion(){
	t_direccion direccion;
	int posicionStackAnterior = pcb->sizeContextoActual-2;
	int posicionUltimaVariable = pcb->contextoActual[posicionStackAnterior]->sizeVars-1;
	return proximaDireccion(posicionStackAnterior, posicionUltimaVariable);

}

t_direccion proximaDireccion(int posStack, int posUltVar){
	t_direccion direccion;
	int offset = pcb->contextoActual[posStack]->vars[posUltVar]->direccion.offset + 4;
		if(offset>tamanioPag){
			direccion.pagina = pcb->contextoActual[posStack]->vars[posUltVar]->direccion.pagina + 1;
			direccion.offset = 0;
			direccion.size=4;
			return direccion;
		}else{
			direccion.pagina = pcb->contextoActual[posStack]->vars[posUltVar]->direccion.pagina;
			direccion.offset = offset;
			direccion.size=4;
		}
		return direccion;
}
