/*
 * primitivas.c
 *
 *  Created on: 26/4/2016
 *      Author: Franco Albornoz
 */

#include "funcionesCPU.h"

t_puntero definirVariable(t_nombre_variable identificador_variable)
{
	t_direccion *direccion_variable= malloc(sizeof(t_direccion));
	t_variable *variable= malloc(sizeof(t_variable));
	t_contexto *contexto; //= malloc(sizeof(t_contexto));
	int posicionStack = pcb->sizeContextoActual-1;
	contexto= (t_contexto*)(list_get(pcb->contextoActual, pcb->sizeContextoActual-1));

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

	char* escribirUMC= malloc(16);
	int valor;
	enviarDirecParaEscribirUMC(escribirUMC, direccion_variable, valor);
	free(escribirUMC);

	if(recibir(umc)->data=="Error"){ //hablar con umc sobre el error
		programaAbortado=1;
	}

	int direccionRetorno = convertirDireccionAPuntero(direccion_variable);
	return (direccionRetorno);

}


t_puntero obtenerPosicionVariable (t_nombre_variable identificador_variable)
{

		int posicionStack=pcb->sizeContextoActual-1;
		t_variable *variable_nueva=malloc(sizeof(t_variable));
		int posMax= (((t_contexto*)(list_get(pcb->contextoActual, posicionStack)))->sizeVars)-1;
		int direccionRetorno;
		while(posMax>=0){
			variable_nueva=((t_variable*)(list_get(((t_contexto*)(list_get(pcb->contextoActual, posicionStack)))->vars, posMax)));
			if(variable_nueva->etiqueta==identificador_variable){
				direccionRetorno = convertirDireccionAPuntero(((t_variable*)(list_get(((t_contexto*)(list_get(pcb->contextoActual, posicionStack)))->vars, posMax)))->direccion);
				free(variable_nueva);
				return(direccionRetorno); //no queda claro en el enunciado que devuelve

			}
			posMax--;
		}
		programaAbortado=1;
		return -1;

}

t_valor_variable dereferenciar(t_puntero direccion_variable)
{
	t_direccion *direccion= malloc(sizeof(t_direccion));
	convertirPunteroADireccion(direccion_variable, direccion);
	char* leerUMC=malloc(12);
	enviarDirecParaLeerUMC(leerUMC, direccion);
	free(leerUMC);
	free(direccion);
	t_paquete *paquete=malloc(sizeof(t_paquete));
	paquete = recibir(umc);
	int valor;
	memcpy(&valor, &paquete->data, 4);
	free(paquete);
	return valor;

}

void asignar(t_puntero direccion_variable,t_valor_variable valor)
{	t_direccion *direccion= malloc(sizeof(t_direccion));
	convertirPunteroADireccion(direccion_variable, direccion);
	char* escribirUMC= malloc(16);
	enviarDirecParaEscribirUMC(escribirUMC, direccion, valor);
	free(escribirUMC);
	free(direccion);
	return;
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable)
{
	char variable_compartida= malloc(sizeof(variable));
	variable_compartida=variable;
	t_paquete* paquete_nuevo;
	int valor;
	enviar(nucleo, 351, sizeof(variable_compartida), variable_compartida);
	paquete_nuevo = recibir(nucleo);
	memcpy(&valor, paquete_nuevo->data, sizeof(int));
	free(paquete_nuevo);
	return valor;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable,t_valor_variable valor)
{
	char *variable_compartida= malloc(4+sizeof(variable));
	memcpy(variable_compartida, &valor, 4);
	memcpy(variable_compartida+4, variable, sizeof(variable));
	enviar(nucleo, 350, sizeof(variable_compartida), variable_compartida);
	free(variable_compartida);
	return valor;
}

void irAlLabel(t_nombre_etiqueta etiqueta)
{	t_puntero_instruccion instruccion;
	instruccion = metadata_buscar_etiqueta(etiqueta, pcb->indiceDeEtiquetas, 1);
	int i;
	int contador=0;
	i=pcb->indiceDeCodigo[contador];
	while(i!=instruccion){
		contador+=2;
		i=pcb->indiceDeCodigo[contador];
	}
	pcb->pc=(contador/2);
	return;
}

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar)
{
	//creamos nuevo contexto

	return;
}

void retornar(t_valor_variable retorno)
{
	printf("Soy la funcion retornar\n");
	return;
}

void imprimir(t_valor_variable valor_mostrar)
{
	enviar(nucleo, 360, sizeof(t_valor_variable), &valor_mostrar);
	return;
}

void imprimirTexto(char*texto)
{	char *texto_mostrar = malloc(sizeof(texto));
	texto_mostrar = texto;
	enviar(nucleo, 361, sizeof(texto_mostrar), texto_mostrar);
	free(texto_mostrar);
	return;
}

void entradaSalida(t_nombre_dispositivo dispositivo,int tiempo)
{	char* nombre_dispositivo=malloc(sizeof(dispositivo));
	nombre_dispositivo = dispositivo;
	enviar(nucleo, 380, sizeof(nombre_dispositivo), nombre_dispositivo);
	enviar(nucleo, 381, 4, &tiempo);
	programaBloqueado=1;
	free(nombre_dispositivo);
	return;
}

void finalizar(){
	t_contexto *contexto_a_finalizar; //= malloc(sizeof(t_contexto));
	contexto_a_finalizar= list_get(pcb->contextoActual, pcb->sizeContextoActual-1);
	while(contexto_a_finalizar->sizeVars != 0){
		free(((t_variable *)list_get(contexto_a_finalizar->vars, contexto_a_finalizar->sizeVars-1))->direccion);
		free(((t_variable *)list_get(contexto_a_finalizar->vars, contexto_a_finalizar->sizeVars-1)));
		list_remove(contexto_a_finalizar->vars, (contexto_a_finalizar->sizeVars)-1);
		contexto_a_finalizar->sizeVars--;
	}
	list_destroy(contexto_a_finalizar->vars);
	free((t_contexto *)list_get(pcb->contextoActual, pcb->sizeContextoActual-1));
	programaFinalizado=1;
	return;


}

void wait_kernel(t_nombre_semaforo identificador_semaforo){
	char* nombre_semaforo=malloc(sizeof(identificador_semaforo));
	nombre_semaforo = identificador_semaforo;
	enviar(nucleo, 341, sizeof(nombre_semaforo), nombre_semaforo);
	t_paquete* paquete = malloc(sizeof(paquete));
	paquete = recibir(nucleo);//devuelve el int con el valor del sem, falta el if para ver si se bloquea o no
	memcpy(&programaBloqueado, &paquete->data, 4); //ver esto
	free(nombre_semaforo);
	free(paquete);
	return;
}

void signal_kernel(t_nombre_semaforo identificador_semaforo){
	char* nombre_semaforo=malloc(sizeof(identificador_semaforo));
	nombre_semaforo = identificador_semaforo;
	enviar(nucleo, 342, sizeof(nombre_semaforo), nombre_semaforo);
	free(nombre_semaforo);
	return;
}

void armarDireccionPrimeraPagina(t_direccion *direccionReal){
	t_direccion *direccion = malloc(sizeof(t_direccion));
	direccion->offset=0;
	direccion->size=4;
	direccion->pagina=primeraPagina();
	memcpy(direccionReal, direccion , sizeof(t_direccion));
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
		if(offset>=tamanioPag){
			direccion->pagina = ((t_variable*)(list_get(((t_contexto*)(list_get(pcb->contextoActual, posStack)))->vars, posUltVar)))->direccion->pagina + 1;
			direccion->offset = 0;
			direccion->size=4;
			memcpy(direccionReal, direccion , sizeof(t_direccion));
			free(direccion);
		}else{
			direccion->pagina = ((t_variable*)(list_get(((t_contexto*)(list_get(pcb->contextoActual, posStack)))->vars, posUltVar)))->direccion->pagina;
			direccion->offset = offset;
			direccion->size=4;
			memcpy(direccionReal, direccion , sizeof(t_direccion));
			free(direccion);
		}

		return;
}

int convertirDireccionAPuntero(t_direccion* direccion){
	int direccion_real,pagina,offset;
	pagina=(direccion->pagina)*tamanioPag;
	offset=direccion->offset;
	direccion_real=pagina+offset;
	return direccion_real;
}

void convertirPunteroADireccion(int puntero, t_direccion* direccion){
	if(tamanioPag>puntero){
		direccion->pagina=0;
		direccion->offset=puntero;
		direccion->size=4;
	}else{
		direccion->pagina = (puntero/tamanioPag);
		direccion->offset = puntero%tamanioPag;
		direccion->size=4;
	}
	return;
}

void enviarDirecParaEscribirUMC(char* UMC, t_direccion* direccion, int valor){

		memcpy(UMC, &direccion->pagina , 4);
		memcpy(UMC+4, &direccion->offset , 4);
		memcpy(UMC+8, &direccion->size , 4);
		memcpy(UMC+12, &valor , 4);
		enviar(umc, 2, 16, UMC);

}

void enviarDirecParaLeerUMC(char* UMC_1, t_direccion* direccion){

		memcpy(UMC_1, &direccion->pagina , 4);
		memcpy(UMC_1+4, &direccion->offset , 4);
		memcpy(UMC_1+8, &direccion->size , 4);
		printf("Cargue direccion: %d %d %d\n",((int*)(UMC_1))[0],((int*)(UMC_1))[1],((int*)(UMC_1))[2]);
		enviar(umc, 1, 12, UMC_1);

}
