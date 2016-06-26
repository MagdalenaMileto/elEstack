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
	//int sigusr1_desactivado=0;
	log= log_create(ARCHIVOLOG, "CPU", 0, LOG_LEVEL_INFO);
	log_info(log,"Iniciando CPU\n");
	char* serializado;
	programaBloqueado = 0;
	programaFinalizado = 0;
	programaAbortado = 0;

	//levantar_configuraciones();



	umc = conectarConUmc();
	printf("holaUMC\n");

	nucleo = conectarConNucleo();
	printf("holanucleo\n");
	t_paquete* datos_kernel=recibir(nucleo);  //una vez que nucleo se conecta con cpu debe mandar t_datos_kernel..

	quantum = ((t_datos_kernel*)(datos_kernel->data))->QUANTUM;
	tamanioPag = ((t_datos_kernel*)(datos_kernel->data))->TAMPAG;
	quantum_sleep = ((t_datos_kernel*)(datos_kernel->data))->QUANTUM_SLEEP;

	sigusr1_desactivado = 1;

	//if (signal(SIGUSR1, sig_handler) == SIG_ERR )
						//log_error(log, "Error al atrapar señal SIGUSR1");

	while(sigusr1_desactivado){

		pcb = malloc(sizeof(t_pcb));
		t_paquete* paquete_recibido = recibir(nucleo);
		pcb = desserializarPCB(paquete_recibido->data);
		log_info(log,"Recibi PCB del nucleo\n");
		printf("recibi PCB\n");
		liberar_paquete(paquete_recibido);

		int pid = pcb->pid;
		enviar(umc, 3, sizeof(int), &pid);
		printf("Envie pid a UMC\n");


		while(quantum && !programaBloqueado && !programaFinalizado && !programaAbortado){

			t_direccion* datos_para_umc = malloc(12);
			crearEstructuraParaUMC(pcb, tamanioPag, datos_para_umc);
			char * lecturaUMC= malloc(12);
			printf("Voy a pedir instruccion\n");
			printf("Direccion= Pag:%d Offset:%d Size:%d\n", datos_para_umc->pagina, datos_para_umc->offset, datos_para_umc->size);
			enviarDirecParaLeerUMC(lecturaUMC, datos_para_umc);
			printf("Pido Instruccion\n");
			free(lecturaUMC);
			free(datos_para_umc);
			t_paquete* instruccion=malloc(sizeof(t_paquete));
			instruccion = recibir(umc);
			printf("Recibi instruccion de UMC\n");
			char* sentencia=instruccion->data;
			//memcpy(sentencia, &instruccion->data, (int)instruccion->tamanio); //chequear si hay una instruccion?
			printf("Recibi sentencia: %s\n", sentencia);
			analizadorLinea(depurarSentencia(strdup(sentencia)), &primitivas, &primitivas_kernel);
			liberar_paquete(instruccion);

			pcb->pc++;
			quantum--;
			usleep(quantum_sleep);

			if (programaBloqueado){
				log_info(log, "El programa salió por bloqueo");
				serializado = serializarPCB(pcb);
				enviar(nucleo, 340, sizeof(serializado), serializado);
				destruirPCB(pcb);
					}

			if (programaAbortado){
				log_info(log, "El programa aborto");
				serializado = serializarPCB(pcb);
				enviar(nucleo, 370, sizeof(serializado), serializado);
				destruirPCB(pcb);
			}

			if (programaFinalizado){
				log_debug(log, "El programa finalizo");
				enviar(nucleo, 320, sizeof(int), &programaFinalizado);
				destruirPCB(pcb);
			}

			if(quantum &&!programaFinalizado&&!programaBloqueado&&!programaAbortado){
				serializado = serializarPCB(pcb);
				enviar(nucleo, 304, sizeof(serializado), serializado);
				destruirPCB(pcb);
			}
		}

		liberar_paquete(datos_kernel);
		free(pcb);
		close(nucleo);
		close(umc);

		return 0;
	}

return 0;

}

//*******************************************FUNCIONES**********************************************************

int conectarConUmc(){
		int umc = conectar_a("localhost", "1200");
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
		int nucleo = conectar_a("localhost", "1202");
		if(nucleo==-1){
			log_info(log,"CPU: No encontre nucleo\n");
			exit (EXIT_FAILURE);
		}
		log_info(log,"CPU: Conecta bien nucleo %d\n", nucleo);
		//bool estado = realizar_handshake(nucleo);
		//if (!estado) {
		//	log_info(log,"CPU:Handshake invalido con nucleo\n");
		//	exit(EXIT_FAILURE);
		//}
		//else{
		//	log_info(log,"CPU:Handshake exitoso con nucleo\n");
		//}
return nucleo;
}

void crearEstructuraParaUMC (t_pcb* pcb, int tamPag, t_direccion* informacion){

	t_direccion* info=malloc(sizeof(t_direccion));
	info->pagina=pcb->indiceDeCodigo [(pcb->pc)*2]/ tamPag;
	info->offset=pcb->indiceDeCodigo [((pcb->pc)*2)];
	info->size=pcb->indiceDeCodigo [((pcb->pc)*2)+1];
	memcpy(informacion, info, 12);
	free(info);
	return;
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

		int i = strlen(sentencia);
		while (string_ends_with(sentencia, "\n")) {
			i--;
			sentencia = string_substring_until(sentencia, i);
		}
		return sentencia;

}

void sig_handler(int signo) {
	if (signo == SIGUSR1) {
		sigusr1_desactivado = 0;
		log_info(log,
		"Se recibió la señal SIGUSR_1, la CPU se cerrara al finalizar la ejecucion actual");
	}
}
