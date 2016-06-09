/*
 * CPU.c
 *
 *  Created on: 9/6/2016
 *      Author: utnso
 */

#include "funcionesCPU.h"

#define ARCHIVOLOG "CPU.log"

#define CONFIG_CPU "config_cpu"
CONF_CPU *config_cpu;
#include <commons/config.h>
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

	int quantum = info_kernel->QUANTUM;

	while(sigusr1_desactivado){

		t_paquete* paquete_recibido = recibirPCB(nucleo);
		t_pcb* pcb = desserializarPCB(paquete_recibido->data);

		t_direccion* datos_para_umc = crearEstructuraParaUMC (pcb->indiceDeCodigo);

		enviar(umc, 404, datos_para_umc->size, datos_para_umc);
	}



}

//*******************************************FUNCIONES**********************************************************

int conectarConUmc(){
		int umc = conectar_a(config_cpu->IP_UMC, config_cpu->PUERTO_UMC);
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
		int nucleo = conectar_a(config_cpu->IP_NUCLEO, config_cpu->PUERTO_NUCLEO);
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


t_direccion*  crearEstructuraParaUMC (int* indice){

	t_direccion* info;
	info->pagina=0;  // como sacar estos valores
	info->offset=0;
	info->size=0;

	return info;

}


t_datos_kernel* desserializarDatosKernel(t_paquete* paquete_kernel); //hacer la deserializacion

void levantar_configuraciones() {

	t_config * archivo_configuracion = config_create("./CPU.confg");

	config_cpu->PUERTO_NUCLEO = config_get_string_value(archivo_configuracion, "PUERTO_NUCLEO");
	config_cpu->IP_NUCLEO = config_get_string_value(archivo_configuracion, "IP_NUCLEO");
	config_cpu->PUERTO_UMC = config_get_string_value(archivo_configuracion, "PUERTO_UMC");
	config_cpu->IP_UMC = config_get_int_value(archivo_configuracion, "IP_UMC");
	config_cpu->STACK_SIZE = config_get_int_value(archivo_configuracion, "STACK_SIZE");
	config_cpu->SIZE_PAGINA = config_get_int_value(archivo_configuracion,"SIZE_PAGINA");

}
