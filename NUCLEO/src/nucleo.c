/*
 * nucleo.c
 *
 *  Created on: 23/4/2016
 *      Author: nico
 */



#include "nucleo.h"


#define ERROR -1



#define CONFIG_NUCLEO "config" //Cambiar esto para eclipse
// #define CONFIG_NUCLEO "config"

/* VARIABLES GLOBALES */
int * CONSTANTE;
CONF_NUCLEO *config_nucleo;

//Colas PLP
t_queue* cola_new;
t_queue* cola_exit;

//Colas PCP
t_queue* cola_exec;
t_queue* cola_ready;
t_queue* cola_block;

pthread_mutex_t mutex_cola_new;

int pidcounter = 0;


// Listas que maneja el PCP
t_queue* cola_CPU_libres;


struct arg_struct {
	int socket;

};


int socketconsola,socketcpu;


//Eliminar todo lo que sea de mas de aca
int listenningSocket, socketCliente, servidorSocket, servidorCPU, clienteSocket, losClientes, clientesCPU, umc, ultimoCPU;

char codigo[200]="#!/usr/bin/ansisop\nbegin\nvariables i,b\ni=1\n:inicio_for\ni=i+1\nprint i\nb=i­10\n:cosita\njnz b inicio_for\njnz b cosita\n#fuera del for\nend";


//char codigo[300] = "#!/usr/bin/ansisop\nbegin\nvariables a, b\na=20\nprint a\nb <­- prueba\nprint b\nprint a\nend\nfunction prueba\nvariables a,b \na=2\nb=16\nprint b\nprint a\na=a+b\nreturn a\nend";


/* FIN DE VARIABLES GLOBALES */





int main() {

	signal(SIGINT, intHandler);

	printf("NUCLEO: INICIÓ\n");


	//Levantar archivo de configuracion
	config_nucleo = malloc(sizeof(CONF_NUCLEO));


	get_config_nucleo(config_nucleo);//Crea y setea el config del kernel


	printf("valor %d\n", config_nucleo->VALOR_SEM[2]);




//MOCKS


	pthread_t mock;
	pthread_create(&mock, NULL, hilo_mock, NULL);

	sleep(3);



	//Inicializaciones -> lo podriamos meter en una funcion externa

	cola_new = queue_create();
	cola_exec = queue_create();
	cola_ready = queue_create();
	cola_block = queue_create();
	cola_exit = queue_create();
	cola_CPU_libres = queue_create();

	pthread_mutex_init(&mutex_cola_new, NULL);



	pthread_t thPCP, thPLP, thCONEXIONES_CPU, thCONEXIONES_CONSOLA;

	pthread_create(&thCONEXIONES_CONSOLA, NULL, hilo_HANDLER_CONEXIONES_CONSOLA, NULL);
	pthread_create(&thCONEXIONES_CPU, NULL, hilo_HANDLER_CONEXIONES_CPU, NULL);


	pthread_create(&thPCP, NULL, hilo_PCP, NULL);
	pthread_create(&thPLP, NULL, hilo_PLP, NULL);


	conectarUmc();

	pthread_join(thCONEXIONES_CONSOLA, NULL);
	pthread_join(thCONEXIONES_CPU, NULL);

	pthread_join(thPCP, NULL);
	pthread_join(thPLP, NULL);




	//Terminamos
}



void conectarUmc(void) {

	//No me pude conectar?
	umc = conectar_a(config_nucleo->IP_UMC, config_nucleo->PUERTO_UMC);


	//Handshake



}

t_proceso* dameProceso(t_queue *cola, int sock ) {
	int a = 0, t;
	t_proceso *w;

	while (w = (t_proceso*)list_get(cola->elements, a)) {

		if (w->socket_CPU == sock) return (t_proceso*)list_remove(cola->elements, a);
		a++;
	}

	return NULL;
}





void *hilo_PLP(void *arg) {
	t_proceso *proceso;

//Hay un proceso en new mando a ready
//hacerlo con semafoto para que bloquee

	while (1) {
		if (queue_size(cola_new) != 0) {
			proceso = queue_pop(cola_new);
			printf("Nucleo: Saco proceso  %d new, mando a ready\n", proceso->pcb->pid);
			queue_push(cola_ready, proceso);
		}
	}

}




void mandarAEjecutar(t_proceso *proceso, int sock) {
	t_pcb *pcbSerializado;
	pcbSerializado = (t_pcb*)serializarPCB(proceso->pcb);
	enviar(sock, 303, pcbSerializado->sizeTotal, pcbSerializado);
	free(pcbSerializado);
	proceso->socket_CPU=sock;
	queue_push(cola_exec, proceso);

}


int handlerErrorCPU(int error, int sock) {
	/*
	t_proceso *proceso;
	if (error == -1) {
		if ((proceso = dameProceso(cola_exec, sock)) || (proceso = dameProceso(cola_ready, sock)) || (proceso = dameProceso(cola_block, sock)) || (proceso = dameProceso(cola_new, sock)) || (proceso = dameProceso(cola_exit, sock))) {
			enviar_id(proceso->socket_CONSOLA, 108);
			return -1;
		}
	}
	return 0;*/
}


void *hilo_PCP(void *arg) {

	t_proceso *proceso; int sock;
	while (1) {

		if (queue_size(cola_ready) != 0 && queue_size(cola_CPU_libres) != 0) {


			sock = (int)queue_pop(cola_CPU_libres);
			proceso = queue_pop(cola_ready);
			printf("Nucleo: Saco proceso %d ready, mando a exec\n", proceso->pcb->pid);

			
			mandarAEjecutar(proceso, sock);
			

			
		}



		//Do something yoooo

		usleep(1000);
	}

}


t_proceso* crearPrograma(int sock) {


	t_proceso* procesoNuevo;
	t_pcb *pcb;
	pcb=malloc(sizeof(t_pcb));
	procesoNuevo = malloc(sizeof(t_proceso));
	procesoNuevo->pcb = pcb;
	procesoNuevo->pcb->pid = pidcounter;
	procesoNuevo->socket_CONSOLA = sock;
	pidcounter++;
	return procesoNuevo;

}


void mandarCodigoAUmc(char* codigo, int size, t_proceso *proceso) {


//borrar hay cosas de mas aca

	int cuantasPaginasCodigo, estado, i;
	t_header header;

	int *indiceCodigo;
	int tamanoIndiceCodigo;

	t_medatada_program* metadata_program;
	metadata_program = metadata_desde_literal(codigo);


//Cuento cuantas paginas me va a llevar el codigo en la umc
	proceso->pcb->paginasDeCodigo = ceil((double)size / (double)config_nucleo->SIZE_PAGINA);

//Tamaño del indice de etiquetas
	proceso->pcb->sizeIndiceDeCodigo =  (metadata_program->instrucciones_size);

	proceso->pcb->indiceDeCodigo = malloc(proceso->pcb->sizeIndiceDeCodigo*2*sizeof(int));

//Creamos el indice de codigo
	for (i = 0; i < metadata_program->instrucciones_size; i++) {
		//printf("Instruccion %.*s",metadata_program->instrucciones_serializado[i].offset,codigo+metadata_program->instrucciones_serializado[i].start);
		proceso->pcb->indiceDeCodigo[i * 2] = metadata_program->instrucciones_serializado[i].start;
		proceso->pcb->indiceDeCodigo[i * 2 + 1] = metadata_program->instrucciones_serializado[i].offset;
	}



//Hacerlo con memcopy 
	proceso->pcb->sizeIndiceDeEtiquetas = metadata_program->etiquetas_size;
	proceso->pcb->indiceDeEtiquetas=malloc(proceso->pcb->sizeIndiceDeEtiquetas*sizeof(char));
	memcpy(proceso->pcb->indiceDeEtiquetas,metadata_program->etiquetas,proceso->pcb->sizeIndiceDeEtiquetas*sizeof(char));		



//Ver esto
	proceso->pcb->contextoActual = malloc(1*sizeof(proceso->pcb->contextoActual));
	t_contexto *contextocero;
	contextocero=malloc(sizeof(t_contexto));
		printf("oo\n");
	proceso->pcb->contextoActual[0]=contextocero;

	printf("pp\n");
	proceso->pcb->contextoActual[0]->sizeArgs=0;
		proceso->pcb->contextoActual[0]->pos=0;
			printf("mau%d\n",proceso->pcb->contextoActual[0]->pos);
	//contextocero->sizeVars=0;

	proceso->pcb->sizeContextoActual=1;

	metadata_destruir(metadata_program);
	printf("ARRARARA\n");

}



/********************************************************************************
*********************************************************************************
*********************************************************************************
************************CONSOLA STUFF********************************************
*********************************************************************************
*********************************************************************************
*********************************************************************************
*/

void *hilo_CONEXION_CONSOLA(void *arg) {

	struct arg_struct *args = (struct arg_struct *)arg;
	int estado;
	t_proceso* proceso;

/*
	if (handshake(args->socket, 101, 102) != 1) {
		printf("NUCLEO:Handshake invalido consola %d\n", args->socket);
		// return;
	}
	*/
	printf("NUCLEO:Handshake valido consola, creando proceso %d\n", args->socket);

	proceso = crearPrograma(args->socket);

	while (1) {
		t_paquete* paquete; // Esto tiene que ser un puntero cambiar.....



		paquete = recibir(args->socket);

		
		switch (paquete->codigo_operacion) {
		case 103:

			mandarCodigoAUmc(paquete->data, paquete->tamanio, proceso);
			printf("**Paginas de codigo:%s\n",proceso->pcb->indiceDeEtiquetas);
			pthread_mutex_lock(&mutex_cola_new);
			queue_push(cola_new, proceso);
			pthread_mutex_unlock(&mutex_cola_new);
			break;

		}



	}

}



void *hilo_HANDLER_CONEXIONES_CONSOLA(void *arg) {

	int servidorSocket, socketCliente;
	struct arg_struct *args; //Probar de sacar afuera esto?


	socketconsola = socket_escucha("localhost", config_nucleo->PUERTO_PROG);
	listen(socketconsola, 1024);
	


	listen(socketconsola, 5);   //Aca maximas conexiones, ver de cambiar?

	while (1) {
		socketCliente = aceptar_conexion(socketconsola);

		args = malloc(sizeof(struct arg_struct));//cuando se termine el proceso hacer un free de esto
		args->socket = socketCliente;

		pthread_t thCONEXION_CONSOLA;
		pthread_create(&thCONEXION_CONSOLA, NULL, hilo_CONEXION_CONSOLA, (void *)args);

		printf("NUCLEO: Acepte consola %d\n", socketCliente);
	}

}


/********************************************************************************
*********************************************************************************
*********************************************************************************
************************CPU STUFF************************************************
*********************************************************************************
*********************************************************************************
*********************************************************************************
*/

void *hilo_CONEXION_CPU(void *arg) {

	struct arg_struct *args = (struct arg_struct *)arg;
	t_proceso* proceso;
	queue_push(cola_CPU_libres, (void*)args->socket);
	t_paquete * elPaquete;
	while (1) {


		elPaquete = recibir(args->socket);
	printf("CRASH2 %d %d\n",elPaquete->codigo_operacion,args->socket);

		switch (elPaquete->codigo_operacion) {
		case 304:
			proceso = dameProceso(cola_exec, args->socket);
			t_pcb *temp;
			temp=desserializarPCB(elPaquete->data);
			printf("NUCLEO: Recibi proceso %d por fin de quantum, encolando en cola ready\n",proceso->pcb->pid);
		
			int i;for(i=0;i<temp->sizeContextoActual;i++)
				printf("POS: %d\n",temp->contextoActual[i]->pos);
			
			proceso->pcb=temp;
			queue_push(cola_ready, proceso);
			queue_push(cola_CPU_libres, (void *)args->socket);
			printf("Paginas de codigo:%s\n",proceso->pcb->indiceDeEtiquetas);
			break;

		case 320:
			proceso = dameProceso(cola_exec, args->socket);
			printf("NUCLEO: Recibi proceso %d por fin de ejecucion, encolando en cola exit\n", proceso->pcb->pid);
			queue_push(cola_exit, proceso);
			queue_push(cola_CPU_libres, (void *)args->socket);
			break;

		case 340:
			//Free de los proceso? ver
			proceso = dameProceso(cola_exec, args->socket);
			printf("NUCLEO: Recibi proceso %d  para mandar a bloquear por IO \n", proceso->pcb->pid);
			//DesSerializar PCB bloqueado
			t_blocked bloqueado;
			if (bloqueado.IO_offset) {
				//El bloqueo es por IO

				int tiempoDeBloqueo;

				tiempoDeBloqueo = bloqueado.IO_time * config_nucleo->VALOR_IO[bloqueado.IO_offset];

				if (config_nucleo->VALOR_IO_EXPIRED_TIME[bloqueado.IO_offset] < current_timestamp()) {

					config_nucleo->VALOR_IO_EXPIRED_TIME[bloqueado.IO_offset] = tiempoDeBloqueo + current_timestamp();

				} else {
					config_nucleo->VALOR_IO_EXPIRED_TIME[bloqueado.IO_offset] = tiempoDeBloqueo + config_nucleo->VALOR_IO_EXPIRED_TIME[bloqueado.IO_offset];

				}
				proceso->tiempoBloqueado = config_nucleo->VALOR_IO_EXPIRED_TIME[bloqueado.IO_offset];

				queue_push(cola_block, proceso);



			}

			break;

		}


	}

}




void *hilo_HANDLER_CONEXIONES_CPU(void *arg) {

	int servidorSocket, socketCliente;
	struct arg_struct *args; //Probar de sacar afuera esto?


	socketcpu = socket_escucha("localhost", config_nucleo->PUERTO_CPU);
	listen(socketcpu, 1024);
	


	listen(socketcpu, 5);   //Aca maximas conexiones, ver de cambiar?

	while (1) {
		socketCliente = aceptar_conexion(socketcpu);


		
		args = malloc(sizeof(struct arg_struct));//cuando se termine el proceso hacer un free de esto
		args->socket = socketCliente;

		pthread_t thCONEXION_CPU;
		pthread_create(&thCONEXION_CPU, NULL, hilo_CONEXION_CPU, (void *)args);

		printf("NUCLEO: Acepte nueva CPU %d\n", socketCliente);
	}

}



void intHandler(int dummy) {
	//clrscr();

	//close(clienteSocket);
	close(socketcpu);
	close(socketconsola);
	close(umc);

	printf("NUCLEO: CERRÓ\n");
	printf("cierro Todo...\n\n");
//	sleep(1);
	exit(0);
}



/********************************************************************************
*********************************************************************************
*********************************************************************************
************************MOCKS    ************************************************
*********************************************************************************
*********************************************************************************
*********************************************************************************
*/






int umcMock, clienteUmc;

void *hilo_mock(void *arg) {


	struct sockaddr_in addr;  socklen_t addrlen = sizeof(addr);

	pthread_t thmock_consola, thmock_consola2, thmock_cpu;



	pthread_create(&thmock_consola, NULL, hilo_mock_consola, NULL);
	// pthread_create(&thmock_consola2, NULL, hilo_mock_consola, NULL);
	  pthread_create(&thmock_cpu, NULL, hilo_mock_cpu, NULL);



	umcMock = servidor(1207);
	listen(umcMock, 5);
	clienteUmc = accept(umcMock, (struct sockaddr *) &addr, &addrlen);



	printf("UMCMOCK: Acepte conexion%d\n", clienteUmc);



	t_paquete * paquete_nuevo;

	while (1) {

		paquete_nuevo = recibir(clienteUmc);

		if (paquete_nuevo->codigo_operacion == 204) {
			//printf("UMCMOCK: me pidió %d paginas. Le digo q si\n",*((int*)header.data));
			/*
			headerEnviar.id = 205; int a = 1;
			headerEnviar.data = &a;
			headerEnviar.size = sizeof(int);
			enviar_paquete(clienteUmc, headerEnviar);
			*/

		}


		//	printf("Pase\n");
		


	}




}



void *hilo_mock_consola(void *arg) {
	int consola;

	sleep(4);

	consola = cliente("127.0.0.1", 1209);




	printf("CONSOLAMOCK: Conecté%d\n", consola);


	enviar(consola, 103, sizeof(codigo), codigo);

	/*
	t_header header;
	header.size = sizeof(codigo);
	header.id = 103;
	header.data = codigo;

	enviar_paquete(consola, header);
	*/
	while (1) {}
	// sleep(5);

	


}


void *hilo_mock_cpu(void *arg) {
	int cpu, estado;


	sleep(5);
	cpu = cliente("127.0.0.1", 1202);
	printf("CPUMOCK: Conecté%d\n", cpu);



	t_paquete * paquete_nuevo;

	t_pcb *pcb;


	while (1) {
		paquete_nuevo = recibir(cpu);
		if (paquete_nuevo->codigo_operacion == 303) {
		
			pcb=desserializarPCB(paquete_nuevo->data);
			printf("CPUMOCK: Recibi pcb %d... ejecutando\n",pcb->pid);

			sleep(2);
			t_contexto *contexto;

			contexto = malloc(sizeof(t_contexto));
		
			
			
			contexto->pos=pcb->sizeContextoActual;
	
			agregarContexto(pcb,contexto);

			t_pcb * serializado;
		
			serializado = (t_pcb*)serializarPCB(pcb);

			enviar(cpu, 304, serializado->sizeTotal, serializado );
			printf("CPUMOCK: envie PCB a nucleo\n");
			

		}


	}

}









/********************************************************************************
*********************************************************************************
*********************************************************************************
************************Funciones apoyo  ****************************************
*********************************************************************************
*********************************************************************************
*********************************************************************************
*/




void get_config_nucleo (CONF_NUCLEO *config_nucleo)
{



	t_config *fnucleo = config_create(CONFIG_NUCLEO);

	config_nucleo->PUERTO_PROG = config_get_string_value(fnucleo, "PUERTO_PROG");

	config_nucleo->PUERTO_CPU = config_get_string_value(fnucleo, "PUERTO_CPU");

	config_nucleo->QUANTUM = config_get_int_value(fnucleo, "QUANTUM");
	config_nucleo->QUANTUM_SLEEP = config_get_int_value(fnucleo, "QUANTUM_SLEEP");

	config_nucleo->IO_IDS = config_get_array_value(fnucleo, "IO_IDS");

	config_nucleo->IO_SLEEP = config_get_array_value(fnucleo, "IO_SLEEP");

	config_nucleo->SEM_IDS = config_get_array_value(fnucleo, "SEM_IDS");

	config_nucleo->SEM_INIT = config_get_array_value(fnucleo, "SEM_INIT");



	config_nucleo->SHARED_VARS = config_get_array_value(fnucleo, "SHARED_VARS");

	config_nucleo->STACK_SIZE = config_get_int_value(fnucleo, "STACK_SIZE");



//HASTA ACA LO QUE PIDE EL ENUNCIADO



	config_nucleo->SIZE_PAGINA = config_get_int_value(fnucleo, "SIZE_PAGINA");
	config_nucleo->IP_UMC = config_get_string_value(fnucleo, "IP_UMC");
	config_nucleo->PUERTO_UMC = config_get_string_value(fnucleo, "PUERTO_UMC");
	//config_destroy(fnucleo);//Ya no lo necesito





	//config_nucleo->VALOR_SHARED_VARS= convertirConfigInt(?????????,config_nucleo->variables_compartidas);
	//Iniciar en 0
	//Hacer

	config_nucleo->VALOR_IO = convertirConfigInt(config_nucleo->IO_SLEEP, config_nucleo->IO_IDS);
	config_nucleo->VALOR_SEM = convertirConfigInt(config_nucleo->SEM_INIT, config_nucleo->SEM_IDS);

	config_nucleo->VALOR_IO_EXPIRED_TIME = punteroConCero(config_nucleo->IO_IDS);



	return;
}

long long *punteroConCero(char **ana1) {

	int i;
	long long *resul;

	resul = malloc(((strlen((char*)ana1)) / sizeof(char*)) * sizeof(long long));

	for (i = 0; i < (strlen((char*)ana1)) / sizeof(char*); i++) {
		resul[i] = 0;

	}
	return resul;



}

int *convertirConfigInt(char **ana1, char **ana2) {

	int i;
	int *resul;

	/*
	if((strlen(ana1))/sizeof(char*)!=(strlen(ana1))/sizeof(char*))
	{exit(EXIT_FAILURE);//Error en arhcivo conf
	}
	*/
	resul = malloc(((strlen((char*)ana1)) / sizeof(char*)) * sizeof(int));
	for (i = 0; i < (strlen((char*)ana1)) / sizeof(char*); i++) {
		resul[i] = atoi(ana1[i]);

	}
	return resul;

}




long long current_timestamp(void) {
	struct timeval te;
	gettimeofday(&te, NULL);
	long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000;
	return milliseconds;
}


void printbuf(const char* buffer, int len) {
	int i;
	printf("\x1b[31m INICIO PRINT\n");
    for ( i = 0; i < len; ++i)
    	if(buffer[i]>=48&&buffer[i]<=122) printf("%c", buffer[i]);
    else  printf("*");

    printf("\nFIN PRINT\n\x1b[0m");
}


