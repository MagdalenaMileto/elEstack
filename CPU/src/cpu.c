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

/* El programa recibe la IP y puerto del nucleo como primer y segundo parametros
 * y como tercer y cuarto parametros la direccion IP y puerto de la umc.
 */

int main(int argc,char **argv) {

	if (argc != 5) {
		perror("No se ingreso la cantidad de parametros correspondientes");
		return EXIT_FAILURE;
	}

	int maxfd = 3;				// Indice de maximo FD
	char buffer[100];			// Bufer para send/recv

	struct timeval tv;			// Estructura para select()
	tv.tv_sec = 2;
	tv.tv_usec = 500000;

	fd_set readfds,masterfds;	// Estructuras para select()
	FD_ZERO(&readfds);
	FD_ZERO(&masterfds);

	log= log_create(ARCHIVOLOG, "CPU", 0, LOG_LEVEL_INFO);

			log_info(log,"Iniciando CPU\n");
		    //Levantar archivo de configuracion
			config_cpu = malloc(sizeof(CONF_CPU));
			get_config_cpu(config_cpu);//Crea y setea el config del cpu

			//se conecta a nucleo
			int nucleo = conectarConNucleo();


	FD_SET(nucleo,&masterfds);	// Se agrega socket a la lista de fds

	//se conecta con la UMC
	int umc = conectarConUmc();


	FD_SET(umc,&masterfds);		// Se agrega socket a la lista de fds

	t_pcb pcb;					//Declaracion e inicializacion del PCB
	bzero(&pcb,sizeof(pcb));

	AnSISOP_kernel primitivas_kernel;
	primitivas_kernel.AnSISOP_signal = (void*)&signal;
	primitivas_kernel.AnSISOP_wait = (void*)&wait;

	AnSISOP_funciones primitivas;
	primitivas.AnSISOP_asignar = (void*)&asignar;
	primitivas.AnSISOP_asignarValorCompartida = (void*)&asignarValorCompartida;
	primitivas.AnSISOP_definirVariable = (void*)&definirVariable;
	primitivas.AnSISOP_dereferenciar = (void*)&dereferenciar;
	primitivas.AnSISOP_entradaSalida = (void*)&entradaSalida;
	primitivas.AnSISOP_imprimir = (void*)&imprimir;
	primitivas.AnSISOP_imprimirTexto = (void*)&imprimirTexto;
	primitivas.AnSISOP_irAlLabel = (void*)&irAlLabel;
	primitivas.AnSISOP_obtenerPosicionVariable = (void*)&obtenerPosicionVariable;
	primitivas.AnSISOP_obtenerValorCompartida = (void*)&obtenerValorCompartida;
	primitivas.AnSISOP_retornar = (void*)&retornar;

	while(1)
	{
		readfds = masterfds;	// Copio el struct con fds al auxiliar para read
		select(maxfd+1,&readfds,NULL,NULL,&tv);
		if (FD_ISSET(nucleo, &readfds))		// Si el nucleo envio algo
		{
			recv(nucleo,buffer,sizeof(buffer),MSG_DONTWAIT);
			printf("CPU: El nucleo informa lo siguiente: %s\nMensaje enviado a la UMC.\n",buffer);
			send(umc,buffer,sizeof(buffer),0);
			FD_CLR(nucleo,&masterfds);
			analizadorLinea("a = b + c",&primitivas,&primitivas_kernel);
			printf("CPU: Cierra\n");
			return EXIT_SUCCESS;
		}
		//recv(nucleo,&pcb,sizeof(pcb),0);		// El CPU nos envia una copia del PCB o nos envia su direccion en la UMC?
		close(nucleo);
		close(umc);
	}

	close(nucleo);
	close(umc);
	return EXIT_SUCCESS;

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

		int umc = cliente("127.0.0.1", 1207); //tendria que leer del archivo de conf de nucleo

		if(umc==0){
				log_info(log,"CPU: No encontre memoria\n");
				exit (EXIT_FAILURE);
		}

		log_info(log,"CPU: Conecta bien memoria %d\n", umc);

		//hacer el handshake???

return umc;
}

int conectarConNucleo(){

		int nucleo = cliente("127.0.0.1", 9997); //tendria que leer del archivo de conf del nucleo

		if(nucleo==0){
				log_info(log,"CPU: No encontre nucleo\n");
				exit (EXIT_FAILURE);
		}

		log_info(log,"CPU: Conecta bien nucleo %d\n", nucleo);

        int estado;
        estado=handshake(nucleo, 302,301);

        if(estado==1){
        	log_info(log,"CPU:Handshake exitoso con nucleo\n");
        }else{
        	log_info(log,"CPU:Handshake invalido con nucleo\n");
        }

return nucleo;
}
