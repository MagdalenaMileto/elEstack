/*
 * primitivas.c
 *
 *  Created on: 26/4/2016
 *      Author: Franco Albornoz
 */

#include "primitivas.h"
//#include <stdlib.h>
//#include "../../COMUNES/estructurasControl.h"
//#include "funcionesCPU.h"

//extern int programaFinalizado, programaBloqueado, programaAbortado;

t_puntero definirVariable(t_nombre_variable identificador_variable)
{
	t_direccion *direccion_variable= malloc(sizeof(t_direccion));
	t_variable *variable= malloc(sizeof(t_variable));
	t_contexto *contexto= malloc(sizeof(t_contexto));
	int posicionStack = pcb->sizeContextoActual-1;
	contexto= list_get(pcb->contextoActual, pcb->sizeContextoActual-1);

	if(pcb->sizeContextoActual==1 &&  contexto->sizeVars==0 ){

		armarDireccionPrimeraPagina(direccion_variable);
		variable->etiqueta=identificador_variable;
		variable->direccion=direccion_variable;
		list_add(contexto->vars, variable);
		contexto->pos=0;
		contexto->sizeVars++;

	}

	else if(contexto->sizeVars == 0 && (pcb->sizeContextoActual)>1){
		//La posicion va a estar definida cuando se llama a la primitiva funcion

		armarDirecccionDeFuncion(direccion_variable);
		variable->etiqueta=identificador_variable;
		variable->direccion=direccion_variable;
		list_add(contexto->vars, variable);
		contexto->sizeVars++;

	}

	else {
		armarProximaDireccion(direccion_variable);
		variable->etiqueta=identificador_variable;
		variable->direccion=direccion_variable;
		list_add(contexto->vars, variable);
		contexto->sizeVars++;
	}

	enviar(umc, 404, sizeof(t_direccion), direccion_variable);

	//if(recibir(umc)->data=="Error"){ //hablar con umc sobre el error
		//programaAbortado=1;
	//}

	return direccion_variable;

}


t_puntero obtenerPosicionVariable (t_nombre_variable identificador_variable)
{
	int posicionStack=pcb->sizeContextoActual-1;
	t_variable *variable_nueva=malloc(sizeof(t_variable));
	int posMax= (((t_contexto*)(list_get(pcb->contextoActual, posicionStack)))->sizeVars)-1;
	while(posMax>=0){
		variable_nueva=((t_variable*)(list_get(((t_contexto*)(list_get(pcb->contextoActual, posicionStack)))->vars, posMax)));
		if(variable_nueva->etiqueta==identificador_variable){
			free(variable_nueva);
			return (((t_variable*)(list_get(((t_contexto*)(list_get(pcb->contextoActual, posicionStack)))->vars, posMax)))->direccion); //no queda claro en el enunciado que devuelve

		}
		posMax--;
	}
//	programaAbortado=1;
	return -1;

}

t_valor_variable dereferenciar(t_puntero direccion_variable)
{
	t_direccion *direccion= malloc(sizeof(t_direccion));
	direccion = &direccion_variable;
	enviar(umc, 351, sizeof(t_direccion), direccion);
	free(direccion);
	t_paquete *paquete=malloc(sizeof(t_paquete));
	paquete = recibir(umc);
	int valor;
	memcpy(valor, paquete->data, 4);
	free(paquete);
	return valor;

}

void asignar(t_puntero direccion_variable,t_valor_variable valor)
{	t_direccion *direccion= malloc(sizeof(t_direccion));
	direccion = &direccion_variable;
	int *valor_real= malloc(sizeof(t_valor_variable));
	valor_real= &valor;
	enviar(umc, 355, sizeof(t_direccion), direccion); //ver cod de op con umc
	enviar(umc, 356, sizeof(t_valor_variable), valor_real); //codigo de op valor
	free(direccion);
	free(valor_real);
	return;
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

void imprimir(t_valor_variable valor_mostrar)
{
	int *valor = malloc(sizeof(t_valor_variable));
	valor=&valor_mostrar;
	enviar(nucleo, 360, sizeof(t_valor_variable), valor);
	free(valor);
	return;
}

void imprimirTexto(char*texto)
{	char *texto_mostrar = malloc(sizeof(texto));
	texto_mostrar = texto;
	enviar(nucleo, 360, sizeof(texto_mostrar), texto_mostrar); //preguntar si esta bien mandando el texto
	free(texto_mostrar);
	return;
}

int entradaSalida(t_nombre_dispositivo dispositivo,int tiempo)
{
	printf("Soy la funcion entradaSalida\n");
	return 0;
}

void finalizar(){
	t_contexto *contexto_a_finalizar= malloc(sizeof(t_contexto));
	contexto_a_finalizar= list_get(pcb->contextoActual, pcb->sizeContextoActual-1);
	while(contexto_a_finalizar->sizeVars != 0){
		free(((t_variable *)list_get(contexto_a_finalizar->vars, contexto_a_finalizar->sizeVars-1))->direccion);
		free(((t_variable *)list_get(contexto_a_finalizar->vars, contexto_a_finalizar->sizeVars-1)));
		list_remove(contexto_a_finalizar->vars, (contexto_a_finalizar->sizeVars)-1);
		contexto_a_finalizar->sizeVars--;
	}
	list_destroy(contexto_a_finalizar->vars);
	free(contexto_a_finalizar);
	free((t_contexto *)list_get(pcb->contextoActual, pcb->sizeContextoActual-1));
	//programaFinalizado=1;
	return;


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

void armarDireccionPrimeraPagina(t_direccion *direccionReal){
	t_direccion *direccion = malloc(sizeof(t_direccion));
	direccion->offset=0;
	direccion->size=4;
	direccion->pagina=primeraPagina();
	memcpy(&direccionReal, &direccion , sizeof(t_direccion));
	free(direccion);

	return;
}

void armarProximaDireccion(t_direccion* direccionReal){
	int ultimaPosicionStack = pcb->sizeContextoActual-1;
	int posicionUltimaVariable = ((t_contexto*)(list_get(pcb->contextoActual, ultimaPosicionStack)))->sizeVars-1;
	proximaDireccion(ultimaPosicionStack, posicionUltimaVariable, direccionReal);
	return;
}

int primeraPagina(){
	return ((pcb->indiceDeCodigo[pcb->sizeIndiceDeCodigo-2])+pcb->indiceDeCodigo[pcb->sizeIndiceDeCodigo-1]/ tamanioPag)+2;
}

void armarDirecccionDeFuncion(t_direccion *direccionReal){
	//t_direccion direccion = malloc(sizeof(t_direccion));
	if(((t_contexto*)list_get(pcb->contextoActual, pcb->sizeContextoActual-1))->sizeArgs == 0){
		int posicionStackAnterior = pcb->sizeContextoActual-2;
		int posicionUltimaVariable = ((t_contexto*)(list_get(pcb->contextoActual, pcb->sizeContextoActual-2)))->sizeVars-1;
		proximaDireccion(posicionStackAnterior, posicionUltimaVariable, direccionReal);
	}else{
		int posicionStackActual = pcb->sizeContextoActual-1;
		int posicionUltimoArgumento = ((t_contexto*)(list_get(pcb->contextoActual, pcb->sizeContextoActual-1)))->sizeArgs-1;
		proximaDireccion(posicionStackActual, posicionUltimoArgumento, direccionReal);
	}

	return;
}

void proximaDireccion(int posStack, int posUltVar, t_direccion* direccionReal){
	t_direccion *direccion = malloc(sizeof(t_direccion));
	int offset = ((t_variable*)(list_get(((t_contexto*)(list_get(pcb->contextoActual, posStack)))->vars, posUltVar)))->direccion->offset + 4;
		if(offset>tamanioPag){
			direccion->pagina = ((t_variable*)(list_get(((t_contexto*)(list_get(pcb->contextoActual, posStack)))->vars, posUltVar)))->direccion->pagina + 1;
			direccion->offset = 0;
			direccion->size=4;
			memcpy(&direccionReal, &direccion , sizeof(t_direccion));
			free(direccion);
		}else{
			direccion->pagina = ((t_variable*)(list_get(((t_contexto*)(list_get(pcb->contextoActual, posStack)))->vars, posUltVar)))->direccion->pagina;
			direccion->offset = offset;
			direccion->size=4;
			memcpy(&direccionReal, &direccion , sizeof(t_direccion));
			free(direccion);
		}

		return;
}

