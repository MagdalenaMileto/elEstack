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
	//int posicionStack = pcb->sizeContextoActual-1;
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

	/*if(recibir(umc)->data=="Error"){ //hablar con umc sobre el error, usar stringcompare
		programaAbortado=1;
	}*/

	int direccionRetorno = convertirDireccionAPuntero(direccion_variable);
	log_info(log,"Devuelvo direccion: %d\n", direccionRetorno);
	return (direccionRetorno);

}


t_puntero obtenerPosicionVariable (t_nombre_variable identificador_variable)
{
		log_info(log,"Entre a obtenerVariable");
		int posicionStack=pcb->sizeContextoActual-1;
		t_variable *variable_nueva;
		int posMax= (((t_contexto*)(list_get(pcb->contextoActual, posicionStack)))->sizeVars)-1;
		int direccionRetorno;
		log_info(log,"PosMax= %d\n", posMax);
		while(posMax>=0){
			variable_nueva=((t_variable*)(list_get(((t_contexto*)(list_get(pcb->contextoActual, posicionStack)))->vars, posMax)));
			log_info(log,"Variable: %c\n", variable_nueva->etiqueta);
			if(variable_nueva->etiqueta==identificador_variable){
				direccionRetorno = convertirDireccionAPuntero(((t_variable*)(list_get(((t_contexto*)(list_get(pcb->contextoActual, posicionStack)))->vars, posMax)))->direccion);
				log_info(log,"Obtengo valor de %c: %d %d %d\n", variable_nueva->etiqueta, variable_nueva->direccion->pagina, variable_nueva->direccion->offset, variable_nueva->direccion->size);
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
	memcpy(&valor, paquete->data, 4);
	free(paquete);
	log_info(log,"Valor dereferenciado: %d", valor);
	return valor;

}

void asignar(t_puntero direccion_variable,t_valor_variable valor)
{	t_direccion *direccion= malloc(sizeof(t_direccion));
	convertirPunteroADireccion(direccion_variable, direccion);
	char* escribirUMC= malloc(16);
	log_info(log, "Valor a enviar: %d\n", valor);
	enviarDirecParaEscribirUMC(escribirUMC, direccion, valor);
	free(escribirUMC);
	free(direccion);
	return;
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable)
{	char *variable_compartida= malloc(strlen(variable)+1);
	log_info(log,"Tamanio de variable global %d\n", strlen(variable));
	char* barra_cero="\0";
	memcpy(variable_compartida, variable, strlen(variable));
	memcpy(variable_compartida+(strlen(variable)+1), barra_cero, 1);
	log_info(log,"Obteniendo variable compartida %s\n", variable_compartida);
	t_paquete* paquete_nuevo;
	int valor;
	enviar(nucleo, 351, strlen(variable), variable_compartida); // agregar +1 en el size si se agrega el barra_cero
	paquete_nuevo = recibir(nucleo);
	memcpy(&valor, paquete_nuevo->data, 4);
	log_info(log,"%s vale %d\n", variable_compartida, valor);
	free(variable_compartida);
	log_info(log,"Saliendo para imprimir %d\n", valor);
	return valor;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable,t_valor_variable valor)
{	char *variable_compartida= malloc(5+strlen(variable));
	char* barra_cero="\0";
	memcpy(variable_compartida, &valor, 4);
	memcpy(variable_compartida+4, variable, strlen(variable));
	memcpy(variable_compartida+strlen(variable)+4, barra_cero, 1);
	log_info(log,"Variable %s le asigno %d\n", variable_compartida+4, (int*)variable_compartida[0]);
	enviar(nucleo, 350, 5+strlen(variable), variable_compartida);
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
	/*
	int posicionStack = pcb->sizeContextoActual-1;
	t_contexto *contexto_retorno;
	contexto_retorno = (t_contexto *)(list_get(pcb->contextoActual, posicionStack));
	enviar(umc, 2, sizeof(t_direccion), (t_direccion *)contexto_retorno->sizeVars);
	t_paquete *paquete_recibido = recibir (umc);
	memcpy(&donde_retornar, &paquete_recibido->data, sizeof(t_puntero));

	pcb->pc ++;
	pcb->sizeContextoActual = 0;
	t_list *contexto_nuevo = list_create();
	memcpy(pcb->contextoActual, contexto_nuevo, sizeof(t_list));

	t_list *lista_variables = list_create();
	list_add(contexto_nuevo, (t_list *)lista_variables);
	t_list *lista_argumentos = list_create();
	list_add(contexto_nuevo, (t_list *)lista_argumentos);

	irAlLabel(etiqueta);

	//free(contexto_nuevo); // se liberan las listas? list_create reserva memoria..
	//free(lista_variable);
	//free(lista_argumentos);
	*/
}

void retornar(t_valor_variable retorno)
{
	/*
	int posicionStack = pcb->sizeContextoActual-2;
	t_contexto *contexto_anterior;
	contexto_anterior = (t_contexto *)(list_get(pcb->contextoActual, posicionStack));
	pcb->pc --;
	retorno = contexto_anterior->retPos;
	//finalizar();
	//finalizar o programa abortado??
	*/

}

void imprimir(t_valor_variable valor_mostrar)
{
	log_info(log,"Valor a Imprimir: %d\n", valor_mostrar);
	enviar(nucleo, 360, sizeof(t_valor_variable), &valor_mostrar);
	log_info(log,"Saliendo de imprimir");
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
{	char* estructura_dispositivo=malloc(5+strlen(dispositivo));
	char* barra_cero="\0";
	memcpy(estructura_dispositivo, &tiempo, 4);
	memcpy(estructura_dispositivo+4, dispositivo, strlen(dispositivo));
	memcpy(estructura_dispositivo+strlen(dispositivo)+4, barra_cero, 1);
	log_info(log,"%d\n",(int*)estructura_dispositivo[0]);
	log_info(log,"%s\n",(char*)(estructura_dispositivo+4));
	log_info(log,"tamanio mensaje: %d", 5+strlen(dispositivo));
	log_info(log,"Pido i/o por %d unidades de tiempo al %s\n", (int*)estructura_dispositivo[0], (char*)(estructura_dispositivo+4));
	enviar(nucleo, 380, 5+strlen(dispositivo), estructura_dispositivo);
	programaBloqueado=1;
	free(estructura_dispositivo);
	return;
}

void finalizar(){
	log_info(log,"Entre a finaizar\n");
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
	paquete = recibir(nucleo);//devuelve 0 si no se bloquea, 1 si se bloquea
	memcpy(&programaBloqueado, &paquete->data, 4);
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
	return pcb->paginasDeCodigo;//((pcb->indiceDeCodigo[pcb->sizeIndiceDeCodigo-2])+pcb->indiceDeCodigo[pcb->sizeIndiceDeCodigo-1]/ tamanioPag)+2;
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

void enviarDirecParaEscribirUMC(char* UMC_1, t_direccion* direccion, int valor){

		memcpy(UMC_1, &direccion->pagina , 4);
		memcpy(UMC_1+4, &direccion->offset , 4);
		memcpy(UMC_1+8, &direccion->size , 4);
		memcpy(UMC_1+12, &valor , 4);
		log_info(log,"Quiero escribir en la direccion: %d %d %d %d\n",((int*)(UMC_1))[0],((int*)(UMC_1))[1],((int*)(UMC_1))[2],((int*)(UMC_1))[3]);
		enviar(umc, 2, 16, UMC_1);

}

void enviarDirecParaLeerUMC(char* UMC_1, t_direccion* direccion){

		memcpy(UMC_1, &direccion->pagina , 4);
		memcpy(UMC_1+4, &direccion->offset , 4);
		memcpy(UMC_1+8, &direccion->size , 4);
		log_info(log,"Quiero leer en la direccion: %d %d %d\n",((int*)(UMC_1))[0],((int*)(UMC_1))[1],((int*)(UMC_1))[2]);
		enviar(umc, 1, 12, UMC_1);

}
