/*
 * cpu.c
 *
 *  Created on: 20 de abr. de 2016
 *      Author: Franco Albornoz
 */

#include "funcionesCPU.h"
#define ARCHIVOLOG "CPU.log"

#define CONFIG_CPU "config_cpu"
CONF_CPU *config_cpu;
t_log* log;


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




int main(int argc,char **argv) {

	int maxfd = 3;				// Indice de maximo FD

	struct timeval tv;			// Estructura para select()
	tv.tv_sec = 2;
	tv.tv_usec = 500000;

	fd_set readfds,masterfds;	// Estructuras para select()
	FD_ZERO(&readfds);
	FD_ZERO(&masterfds);

	log= log_create(ARCHIVOLOG, "CPU", 0, LOG_LEVEL_INFO);
	log_info(log,"Iniciando CPU\n");

	config_cpu = malloc(sizeof(CONF_CPU));
	get_config_cpu(config_cpu);//Crea y setea el config del cpu

	int nucleo = conectarConNucleo();
	FD_SET(nucleo,&masterfds);	// Se agrega socket a la lista de fds

	int umc = conectarConUmc();
	FD_SET(umc,&masterfds);		// Se agrega socket a la lista de fds

	t_pcb* pcb;					//Declaracion e inicializacion del PCB
	t_paquete* paquete_recibido;
	t_paquete* paquete_posicion_instruccion;

	while(1)
	{
		readfds = masterfds;	// Copio el struct con fds al auxiliar para read
		select(maxfd+1,&readfds,NULL,NULL,&tv);

		if (FD_ISSET(nucleo, &readfds))		// Si el nucleo envio algo
		{
			paquete_recibido = recibirPCB(nucleo);

			if(paquete_recibido->codigo_operacion==304){

				pcb = desserializarPCB(paquete_recibido->data);
				pcb->pc= pcb->pc +1;
				log_info(log,"Recibi pcb %d... ejecturando\n",pcb->pid);

				paquete_posicion_instruccion = enviarInstruccionAMemoria(umc, pcb);

				FD_CLR(nucleo,&masterfds);
				analizadorLinea("a = b + c",&primitivas,&primitivas_kernel);

				//void ejecutarInstruccion(....quantium);  hacer esta funcion
								/*al ejecutar: actuliza los valores del programa de umc
												acualiza el pc del pcb
												notifica al nucleo que concluyo el quantium
								*/

				free(pcb);
				printf("CPU: Cierra\n");
				return EXIT_SUCCESS;
		}


	close(nucleo);
	close(umc);
	}


	close(nucleo);
	close(umc);
	return EXIT_SUCCESS;

}
}

//***************************************FUNCIONES******************************************

void get_config_cpu (CONF_CPU *config_cpu){

	  t_config *fcpu = config_create(CONFIG_CPU);
	  config_cpu->PUERTO_NUCLEO = config_get_int_value(fcpu,"PUERTO_NUCLEO");
	  config_cpu->IP_NUCLEO = config_get_int_value(fcpu,"IP_NUCLEO");

	  config_cpu->PUERTO_UMC = config_get_int_value(fcpu,"PUERTO_UMC");
	  config_cpu->IP_UMC = config_get_int_value(fcpu,"IP_UMC");

	  config_cpu->STACK_SIZE = config_get_int_value(fcpu,"STACK_SIZE");

	  config_cpu->SIZE_PAGINA = config_get_int_value(fcpu, "SIZE_PAGINA");


	  config_destroy(fcpu);

}

int conectarConUmc(){

		int umc = conectar_a(config_cpu->IP_UMC, config_cpu->PUERTO_UMC);

		if(umc==0){
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

		if(nucleo==0){
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

t_paquete* recibirPCB(int nucleo){

	t_paquete* pcb_recibido= recibir(nucleo);
	pcb_recibido=malloc(sizeof(t_pcb));

return pcb_recibido;
}

t_paquete* enviarInstruccionAMemoria(int umc, t_pcb* pcb){

	enviar(umc, 404, pcb->sizeIndiceDeCodigo, pcb->indiceDeCodigo); //le envio el indice de codigo a umc
	//destruir_pcb(pcb);  hacer esta funcion

	t_paquete* paquete= recibir(umc);									//recibo un paquete con la direc en memoria de la prox sentencia a ejectuar
																		//hay que organizarnos con los C_O de umc a cpu puede ser 403?
	if(paquete->codigo_operacion != 403) log_debug(log,"Error de lectura, cierro la ejecución del programa");
	return paquete;
	liberar_paquete(paquete);
}

