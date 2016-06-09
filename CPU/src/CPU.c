/*
 * CPU.c
 *
 *  Created on: 9/6/2016
 *      Author: utnso
 */

#include "funcionesCPU.h"
#include <commons/config.h>
#define ARCHIVOLOG "CPU.log"
#define CONFIG_CPU "config_cpu"
CONF_CPU config_cpu;

t_log* log;  //en COMUNES tendrian que estar las estructuras del log?


AnSISOP_funciones primitivas = {
		.AnSISOP_definirVariable		= definirVariable,
		.AnSISOP_obtenerPosicionVariable= obtenerPosicionVariable,
		.AnSISOP_dereferenciar			= dereferenciar,
		.AnSISOP_asignar				= asignar,
		.AnSISOP_obtenerValorCompartida = obtenerValorCompartida,
		.AnSISOP_asignarValorCompartida = asignarValorCompartida,
		.AnSISOP_irAlLabel				= irAlLabel,
		.AnSISOP_llamarConRetorno		= llamarFuncion,
		.AnSISOP_retornar				= retornar,
		.AnSISOP_imprimir				= imprimir,
		.AnSISOP_imprimirTexto			= imprimirTexto,
		.AnSISOP_entradaSalida			= entradaSalida,

};
AnSISOP_kernel primitivas_kernel = {
		.AnSISOP_wait					=wait,
		.AnSISOP_signal					=signal,
};


int main(int argc,char **argv){

	int sigusr1_desactivado =1;
	log= log_create(ARCHIVOLOG, "CPU", 0, LOG_LEVEL_INFO);
	log_info(log,"Iniciando CPU\n");

	levantar_configuraciones();

	int umc = conectarConUmc();

	int nucleo = conectarConNucleo();
	t_paquete* datos_kernel=recibir(nucleo);  //una vez que nucleo se conecta con cpu debe mandar t_datos_kernel..
	t_datos_kernel* info_kernel = desserializarDatosKernel(datos_kernel->data);

	while(sigusr1_desactivado){

		int quantum = info_kernel->QUANTUM;
		int tamanioPag = info_kernel->TAMPAG;

		t_paquete* paquete_recibido = recibirPCB(nucleo);
		t_pcb* pcb = desserializarPCB(paquete_recibido->data);
		liberar_paquete(paquete_recibido);

		int pid = pcb->pid;
		enviar(umc, 405, sizeof(int), pid); // codigo 405: cambio de proceso activo NUCLEO - UMC

		int programaBloqueado = 0;
		int programaFinalizado = 0;
		int programaAbortado = 0;

		while(quantum && !programaBloqueado && !programaFinalizado &&
				!programaAbortado){

			t_direccion* datos_para_umc = crearEstructuraParaUMC (pcb, info_kernel);

			enviar(umc, 404, datos_para_umc->size, datos_para_umc);
			t_paquete* instruccion=recibir(umc);
			char* sentencia= instruccion->data;
			analizadorLinea(depurarSentencia(strdup(sentencia)), &primitivas,
					&primitivas_kernel);

			pcb->pc++;
			quantum--;
			usleep(info_kernel->QUANTUM_SLEEP);

			if (programaBloqueado){
				log_info(log, "El programa salió por bloqueo");
					}

			if (programaAbortado){
				log_info(log, "El programa aborto");
					//transformar pcb en *paquete y enviar a nucleo
					//enviar(nucleo, 306, sizeof(t_paquete*), ); //codigo de ope 306, pcb abortado
			}

			if (programaFinalizado){
				log_debug(log, "El programa finalizo");
			}

			if(quantum &&!programaFinalizado&&!programaBloqueado&&!programaAbortado){
				//transformar pcb en *paquete y enviar a nucleo
				//enviar(nucleo, 307, sizeof(t_paquete*), ); //codigo de ope 307, pcb salio por quantum
			}
		}

		close(nucleo);
		close(umc);

		return 0;
	}



}

//*******************************************FUNCIONES**********************************************************

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
		int nucleo = conectar_a(config_cpu.IP_NUCLEO, config_cpu.PUERTO_NUCLEO);
		if(nucleo==-1){
			log_info(log,"CPU: No encontre nucleo\n");
			exit (EXIT_FAILURE);
		}
		log_info(log,"CPU: Conecta bien nucleo %d\n", nucleo);
		bool estado = realizar_handshake(nucleo);
		if (!estado) {
			log_info(log,"CPU:Handshake invalido con nucleo\n");
			exit(EXIT_FAILURE);
		}
		else{
			log_info(log,"CPU:Handshake exitoso con nucleo\n");
		}
return nucleo;
}


t_paquete* recibirPCB(int nucleo){   //al recibir hace un malloc, tendriamos que liberarlo o no?
	t_paquete* pcb_recibido= recibir(nucleo);
	pcb_recibido=malloc(sizeof(t_pcb));
return pcb_recibido;
}


t_direccion*  crearEstructuraParaUMC (t_pcb* pcb, t_datos_kernel* info_kernel){

	t_direccion* info;
	info->pagina=pcb->indiceDeCodigo [pcb->pc][0]/ info_kernel->TAMPAG;
	info->offset=pcb->indiceDeCodigo [pcb->pc][0];
	info->size=pcb->indiceDeCodigo [pcb->pc][1];
	return info;
}

t_datos_kernel* desserializarDatosKernel(char* paquete_kernel){

	t_datos_kernel* info = malloc(sizeof(t_datos_kernel));
	memcpy(info, paquete_kernel, sizeof(t_datos_kernel));
	paquete_kernel += sizeof(t_datos_kernel);

	info->QUANTUM = malloc(sizeof(int));
	memcpy(info->QUANTUM, paquete_kernel, sizeof(int));
	paquete_kernel += sizeof(int);

	info->QUANTUM_SLEEP = malloc(sizeof(int));
	memcpy(info->QUANTUM_SLEEP, paquete_kernel, sizeof(int));
	paquete_kernel += sizeof(int);


	info->TAMPAG = malloc(sizeof(int));
	memcpy(info->TAMPAG, paquete_kernel, sizeof(int));
	paquete_kernel += sizeof(int);

	return info;
}



void levantar_configuraciones() {

	t_config * archivo_configuracion = config_create("./CPU.confg");

	config_cpu.PUERTO_NUCLEO = config_get_string_value(archivo_configuracion, "PUERTO_NUCLEO");
	config_cpu.IP_NUCLEO = config_get_string_value(archivo_configuracion, "IP_NUCLEO");
	config_cpu.PUERTO_UMC = config_get_string_value(archivo_configuracion, "PUERTO_UMC");
	config_cpu.IP_UMC = config_get_int_value(archivo_configuracion, "IP_UMC");
	config_cpu.STACK_SIZE = config_get_int_value(archivo_configuracion, "STACK_SIZE");
	config_cpu.SIZE_PAGINA = config_get_int_value(archivo_configuracion,"SIZE_PAGINA");

}


char* depurarSentencia(char* sentencia){
	return sentencia; //Aca hay que sacarle el /n del final a la sentencia
}
