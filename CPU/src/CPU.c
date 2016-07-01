/*
 * CPU.c
 *
 *  Created on: 9/6/2016
 *      Author: utnso
 */

#include "funcionesCPU.h"
#include <commons/config.h>
#define ARCHIVOLOG "CPU.log"
CONF_CPU config_cpu;
int sigusr1_desactivado;

AnSISOP_funciones primitivas = {
		.AnSISOP_definirVariable		= definirVariable,
		.AnSISOP_obtenerPosicionVariable= obtenerPosicionVariable,
		.AnSISOP_dereferenciar			= dereferenciar,
		.AnSISOP_asignar				= asignar,
		.AnSISOP_obtenerValorCompartida = obtenerValorCompartida,
		.AnSISOP_asignarValorCompartida = asignarValorCompartida,
		.AnSISOP_irAlLabel				= irAlLabel,
		.AnSISOP_llamarConRetorno		= llamarConRetorno,
		.AnSISOP_retornar				= retornar,
		.AnSISOP_imprimir				= imprimir,
		.AnSISOP_imprimirTexto			= imprimirTexto,
		.AnSISOP_entradaSalida			= entradaSalida,
		.AnSISOP_finalizar				= finalizar,

};
AnSISOP_kernel primitivas_kernel = {
		.AnSISOP_wait					=wait_kernel,
		.AnSISOP_signal					=signal_kernel,
};


int main(int argc,char **argv){
	int sigusr1_desactivado=1;
	signal(SIGUSR1, sig_handler);
	log= log_create(ARCHIVOLOG, "CPU", 0, LOG_LEVEL_INFO);
	log_info(log,"Iniciando CPU\n");
	char* serializado;
	pthread_mutex_init(&mutex_pcb, NULL);

	levantar_configuraciones();



	umc = conectarConUmc();

	nucleo = conectarConNucleo();
	t_paquete* datos_kernel=recibir(nucleo);

	quantum = ((t_datos_kernel*)(datos_kernel->data))->QUANTUM;
	tamanioPag = ((t_datos_kernel*)(datos_kernel->data))->TAMPAG;
	quantum_sleep = ((t_datos_kernel*)(datos_kernel->data))->QUANTUM_SLEEP;
	stack_size=	((t_datos_kernel*)(datos_kernel->data))->STACK_SIZE;
	log_info(log,"Quantum: %d TamPag: %d Quantum Sleep: %d Stack size: %d Var Max: %d\n", quantum, tamanioPag, quantum_sleep, stack_size, var_max);



	while(sigusr1_desactivado){
		log_info(log,"sigusr1: %d", sigusr1_desactivado);
		programaBloqueado = 0;
		programaFinalizado = 0;
		programaAbortado = 0;
		int quantum_aux=quantum;
		log_info(log,"Esperando Pcb\n");
		pcb = malloc(sizeof(t_pcb));
		t_paquete* paquete_recibido = recibir(nucleo);
		//sleep(4);
		log_info(log,"Deserializando PCB\n");
		pcb = desserializarPCB(paquete_recibido->data);
		log_info(log,"Recibi PCB del nucleo con el program counter en: %d y SizeContextoActual en %d\n", pcb->pc, pcb->sizeContextoActual);
		liberar_paquete(paquete_recibido);
		log_info(log,"Pcb pid: %d\n",pcb->pid); //ver
		int pid = pcb->pid;
		var_max=(tamanioPag*(stack_size+pcb->paginasDeCodigo))-1;
		log_info(log, "Enviando pid %d a UMC\n",pcb->pid);
		enviar(umc, 3, sizeof(int), &pid);
		log_info(log, "Envie pid %d a UMC\n",pcb->pid);


		while((quantum_aux!=0) && !programaBloqueado && !programaFinalizado && !programaAbortado){

			t_direccion* datos_para_umc = malloc(12);
			crearEstructuraParaUMC(pcb, tamanioPag, datos_para_umc);
			log_info(log, "Direccion= Pag:%d Offset:%d Size:%d\n", datos_para_umc->pagina, datos_para_umc->offset, datos_para_umc->size);



			log_info(log, "Pido instruccion\n");
			char* sentencia=leer(datos_para_umc->pagina,datos_para_umc->offset, datos_para_umc->size);
			if(sentencia==NULL){
				programaAbortado=1;
				log_info(log,"Cambiando flag programaAbortado a %d\n", programaAbortado);
				//break;
			}else{
			log_info(log, "Recibi instruccion de UMC con tamanio %d\n",  datos_para_umc->size);
			char* barra_cero="\0";
			memcpy(sentencia+( datos_para_umc->size-1), barra_cero, 1);

			log_info(log,"Tamanio sentencia: %d\n", strlen(sentencia));

			log_info(log,"Recibi sentencia: %s\n", depurarSentencia(sentencia));
			analizadorLinea(depurarSentencia(strdup(sentencia)), &primitivas, &primitivas_kernel);
			//liberar_paquete(instruccion);
			//free(lecturaUMC);
			free(datos_para_umc);
			free(sentencia);

			pcb->pc++;
			quantum_aux--;
			usleep(quantum_sleep*1000);
			}
			if (programaBloqueado){
				log_info(log, "El programa salió por bloqueo\n");
				log_info(log, "PC= %d\n", pcb->pc);
				serializado = serializarPCB(pcb);
				enviar(nucleo, 340, ((t_pcb*)serializado)->sizeTotal, serializado);
				destruirPCB(pcb);
					}

			if (programaAbortado){
				log_info(log, "El programa aborto\n");
				log_info(log, "El pcb que sale por aborto tiene pid %d\n",pcb->pid);
				serializado = serializarPCB(pcb);
				enviar(nucleo, 370, ((t_pcb*)serializado)->sizeTotal, serializado);
				destruirPCB(pcb);
			}


			if((quantum_aux==0) &&!programaFinalizado&&!programaBloqueado&&!programaAbortado){

				log_info(log,"Saliendo por fin de quantum\n");
				log_info(log,"SizeContextoActual antes: %d\n", pcb->sizeContextoActual);
				serializado = serializarPCB(pcb);
				log_info(log,"SizeContextoActual despues: %d\n", pcb->sizeContextoActual);
				enviar(nucleo, 304, ((t_pcb*)serializado)->sizeTotal, serializado);
				destruirPCB(pcb);
			}
		}

	}

liberar_paquete(datos_kernel);
free(pcb);
close(nucleo);
close(umc);
exit(EXIT_SUCCESS);

}

//*******************************************FUNCIONES**********************************************************

void sig_handler(int signo) {
	sigusr1_desactivado = 0;
	log_info(log,"Se detecto señal SIGUSR1, la CPU se cerrara al finalizar\n");
	return;
}

char* leer(int pagina,int offset, int tamanio)
{
	if((tamanio+offset)<=tamanioPag){

					char * lecturaUMC2= malloc(12);

					t_direccion *datos_para_umc2=malloc(sizeof(t_direccion));

					datos_para_umc2->offset=offset;

					datos_para_umc2->pagina=pagina;

					datos_para_umc2->size=tamanio;

					enviarDirecParaLeerUMC(lecturaUMC2, datos_para_umc2);

					t_paquete* instruccion2=malloc(sizeof(t_paquete));

					instruccion2 = recibir(umc);
					log_info(log,"Codigo de operacion recibido: %d\n", instruccion2->codigo_operacion);
					if(instruccion2->codigo_operacion==7) return NULL;

					char* sentencia2=malloc(datos_para_umc2->size);
					memcpy(sentencia2, instruccion2->data, datos_para_umc2->size);
					free(lecturaUMC2);
					liberar_paquete(instruccion2);
					return sentencia2;

	}else{

		char* lectura1 = leer(pagina,offset,(tamanioPag-offset));
		if(lectura1==NULL) return NULL;
		char* lectura2 = leer(pagina+1,0,tamanio-(tamanioPag-offset));
		if(lectura2==NULL) return NULL;

		char* nuevo =malloc((tamanioPag-offset)+tamanio-(tamanioPag-offset));
		memcpy(nuevo,lectura1,(tamanioPag-offset));
		memcpy(nuevo+(tamanioPag-offset),lectura2,tamanio-(tamanioPag-offset));
		free(lectura1);
		free(lectura2);
		return nuevo;
	}
}


int conectarConUmc(){
		int umc = conectar_a(config_cpu.IP_UMC, config_cpu.PUERTO_UMC);
		if(umc==-1){
			log_info(log,"CPU: No encontre memoria\n");
			exit (EXIT_FAILURE);
		}
		log_info(log,"CPU: Conecta bien memoria %d\n", umc);
		bool estado = realizar_handshake(umc);
		if (!estado) {
			log_info(log,"CPU:Handshake invalido con memoria\n");
			exit(EXIT_FAILURE);
		}
		else{
			log_info(log,"CPU:Handshake exitoso con memoria\n");
		}
return umc;
}


int conectarConNucleo(){
		int nucleo = conectar_a(config_cpu.IP_NUCLEO , config_cpu.PUERTO_NUCLEO);
		if(nucleo==-1){
			log_info(log,"CPU: No encontre nucleo\n");
			exit (EXIT_FAILURE);
		}
		log_info(log,"CPU: Conecta bien nucleo %d\n", nucleo);
		/*bool estado = realizar_handshake(nucleo);
		if (!estado) {
			log_info(log,"CPU:Handshake invalido con nucleo\n");
			exit(EXIT_FAILURE);
		}
		else{
			log_info(log,"CPU:Handshake exitoso con nucleo\n");
		}
		*/
return nucleo;
}

void crearEstructuraParaUMC (t_pcb* pcb, int tamPag, t_direccion* informacion){

	t_direccion* info=malloc(sizeof(t_direccion));
	log_info(log,"Program counter: %d", pcb->pc);
	log_info(log,"Voy a leer en la posicion de indice de codigo:%d\n",pcb->indiceDeCodigo [(pcb->pc)*2]);
	info->pagina=pcb->indiceDeCodigo [(pcb->pc)*2]/ tamPag;
	info->offset=pcb->indiceDeCodigo [((pcb->pc)*2)]%tamPag;
	info->size=pcb->indiceDeCodigo [((pcb->pc)*2)+1];
	memcpy(informacion, info, 12);
	free(info);
	return;
}


void levantar_configuraciones() {

	t_config * archivo_configuracion = config_create("CPU.confg");
	config_cpu.PUERTO_NUCLEO = config_get_string_value(archivo_configuracion, "PUERTO_NUCLEO");
	config_cpu.IP_NUCLEO = config_get_string_value(archivo_configuracion, "IP_NUCLEO");
	config_cpu.PUERTO_UMC = config_get_string_value(archivo_configuracion, "PUERTO_UMC");
	config_cpu.IP_UMC = config_get_string_value(archivo_configuracion, "IP_UMC");
}


char* depurarSentencia(char* sentencia){

		int i = strlen(sentencia);
		while (string_ends_with(sentencia, "\n")) {
			i--;
			sentencia = string_substring_until(sentencia, i);
		}
		return sentencia;

}

