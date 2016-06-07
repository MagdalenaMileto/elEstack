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
	mensaje_CPU_UMC* mensajeAMandar;

	while(1)
	{
		readfds = masterfds;	// Copio el struct con fds al auxiliar para read
		select(maxfd+1,&readfds,NULL,NULL,&tv);

		if (FD_ISSET(nucleo, &readfds))		// Si el nucleo envio algo
		{
			paquete_recibido = recibirPCB(nucleo);

			if(paquete_recibido->codigo_operacion==304){

				pcb = desserializarPCB(paquete_recibido->data);
				log_info(log,"Recibi pcb %d... ejecturando\n",pcb->pid);
				enviarInstruccionAMemoria(umc, mensajeAMandar);

				FD_CLR(nucleo,&masterfds);
				analizadorLinea("a = b + c",&primitivas,&primitivas_kernel);

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


void enviarInstruccionAMemoria(int umc, mensaje_CPU_UMC* mensajeAMandar)
{
	if(mensajeAMandar->texto==NULL) mensajeAMandar->tamTexto=0;

	void*buffer= malloc(sizeof(instruccion_t)+3*sizeof(uint32_t)+(mensajeAMandar->tamTexto));

	memcpy(buffer,&(mensajeAMandar->instruccion),sizeof(instruccion_t));
	memcpy(buffer+sizeof(instruccion_t),&(mensajeAMandar->pid),sizeof(uint32_t));
	memcpy(buffer+sizeof(instruccion_t)+sizeof(uint32_t),&(mensajeAMandar->parametro),sizeof(uint32_t));
	memcpy(buffer+sizeof(instruccion_t)+2*sizeof(uint32_t),&(mensajeAMandar->tamTexto),sizeof(uint32_t));
	memcpy(buffer+sizeof(instruccion_t)+3*sizeof(uint32_t),mensajeAMandar->texto,mensajeAMandar->tamTexto);

	enviar(umc, 404,sizeof(instruccion_t)+3*sizeof(uint32_t)+(mensajeAMandar->tamTexto),mensajeAMandar);  //puse como codigo de operacion 404// como segundo argumento habia puesto buffer pero me tira un warning

	free(buffer);
}
