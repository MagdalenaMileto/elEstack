/*
 * nucleo.c
 *
 *  Created on: 23/4/2016
 *      Author: nico
 */

#include "nucleo.h"
#define CONFIG_NUCLEO "config" //Cambiar esto para eclipse
#define CONFIG_NUCLEO_OLD "src/config"

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

//Cola IO
t_queue** colas_ios;

timer_t** timers;

//semaforos
sem_t sem_ready;
sem_t sem_cpu;
sem_t sem_new;

//Colas sem
t_queue** colas_semaforos;

pthread_mutex_t mutex_cola_new, mutex_cola_ready, mutex_variables,total,umcm;

pthread_mutex_t mutex_config;

int pidcounter = 0;


// Listas que maneja el PCP
t_queue* cola_CPU_libres;

//Modificar eso, sacarlo
struct arg_struct {
	int socket;

};


int socketconsola, socketcpu;


//Eliminar todo lo que sea de mas de aca
int listenningSocket, socketCliente, servidorSocket, servidorCPU, clienteSocket, losClientes, clientesCPU, umc, ultimoCPU;
int rodo=0;
//char codigo[200] = "#!/usr/bin/ansisop\nbegin\nvariables i,b\ni=1\n:inicio_for\ni=i+1\nprint i\nb=i­10\n:cosita\njnz b inicio_for\njnz b cosita\n#fuera del for\nend";

char codigo[250] = "#!/usr/bin/ansisop\nbegin\nvariables a\na = 4\nprint a\nend";


//char codigo[300] = "#!/usr/bin/ansisop\nbegin\nvariables a, b\na=20\nprint a\nb <­- prueba\nprint b\nprint a\nend\nfunction prueba\nvariables a,b \na=2\nb=16\nprint b\nprint a\na=a+b\nreturn a\nend";


/* FIN DE VARIABLES GLOBALES */

int main() {
	signal(SIGINT, intHandler);
	printf("NUCLEO: INICIÓ\n");

	//Levantar archivo de configuracion
	
	config_nucleo = malloc(sizeof(CONF_NUCLEO));
	
	get_config_nucleo(config_nucleo);//Crea y setea el config del kernel

	//MOCK SACAR UNA VEZ INTEGRADO
	pthread_t mock;
	pthread_create(&mock, NULL, hilo_mock, NULL);
	sleep(3); 

	//Inicializaciones 

	sem_init(&sem_cpu, 0, 0);
	sem_init(&sem_new, 0, 0);
	sem_init(&sem_ready, 0, 0);

	cola_new = queue_create();
	cola_exec = queue_create();
	cola_ready = queue_create();
	cola_block = queue_create();
	cola_exit = queue_create();
	cola_CPU_libres = queue_create();

	pthread_mutex_init(&umcm, NULL);
	pthread_mutex_init(&mutex_cola_new, NULL);
	pthread_mutex_init(&total, NULL);

	pthread_t thPCP, thPLP, thCONEXIONES_CPU, thCONEXIONES_CONSOLA;
	pthread_create(&thCONEXIONES_CONSOLA, NULL, hilo_HANDLER_CONEXIONES_CONSOLA, NULL);
	pthread_create(&thCONEXIONES_CPU, NULL, hilo_HANDLER_CONEXIONES_CPU, NULL);
	pthread_create(&thPCP, NULL, hilo_PCP, NULL);
	pthread_create(&thPLP, NULL, hilo_PLP, NULL);

	conectarUmc();

	//INOTIFY STUFF
	int file_descriptor = inotify_init();
	if (file_descriptor < 0) perror("inotify_init");
	int watch_descriptor = inotify_add_watch(file_descriptor, "src/", IN_CLOSE_WRITE);
	char buffer[1000];
	while(1){
		int length = read(file_descriptor, buffer, 1000);
		if (length < 0) {
			perror("read");
		}
		int offset = 0;
		while (offset < length) {
			struct inotify_event *event = (struct inotify_event *) &buffer[offset];
			if (event->len) {
				if (event->mask & IN_CLOSE_WRITE) {
					if (!(event->mask & IN_ISDIR)) {

					//CAMBIAR ESTO PARA ENTREGA FINAL
					//Deberia hacer un free de todo lo otro

						if (strcmp(event->name, CONFIG_NUCLEO_OLD) == 0){
							printf("NUCLEO: cambio el archivo config\n");
							get_config_nucleo(config_nucleo);//Crea y setea el config del kernel
						}
					}
				}
			}
		offset += sizeof (struct inotify_event) + event->len;
		}
	}
	//No haria falta porque bloquea por el while
	pthread_join(thCONEXIONES_CONSOLA, NULL);
	pthread_join(thCONEXIONES_CPU, NULL);

	pthread_join(thPCP, NULL);
	pthread_join(thPLP, NULL);

	//Terminamos
}



void conectarUmc(void) {
	pthread_mutex_lock(&mutex_config);
	umc = conectar_a(config_nucleo->IP_UMC, config_nucleo->PUERTO_UMC);
	pthread_mutex_unlock(&mutex_config);
	//Tengo que hacer handshake?
}


t_proceso* dameProceso(t_queue *cola, int sock ) {
	int a = 0, t;t_proceso *w;
	while (w = (t_proceso*)list_get(cola->elements, a)) {
		if (w->socket_CPU == sock) return (t_proceso*)list_remove(cola->elements, a);
		a++;
	}
	printf("NO HAY PROCESO\n"); exit(0);
	return NULL;
}



void *hilo_PLP(void *arg) {
	t_proceso *proceso;
	while (1) {
		sem_wait(&sem_new);
		//TODO: poner mutex 
		proceso = queue_pop(cola_new);
		printf("NUCLEO: Saco proceso  %d new, mando a ready\n", proceso->pcb->pid);
		queue_push(cola_ready, proceso);
		sem_post(&sem_ready);
	}
}

void mandarAEjecutar(t_proceso *proceso, int sock) {
	t_pcb *pcbSerializado;
	printf("CHOCOLATE%d\n",proceso->pcb->pc);

	pcbSerializado = (t_pcb*)serializarPCB(proceso->pcb);
	printf("CHOCOLATE%d\n",(t_pcb*)pcbSerializado->pc);

	proceso->socket_CPU = sock;
	//TODO:mutex
	queue_push(cola_exec, proceso);
	enviar(sock, 303, pcbSerializado->sizeTotal, (char*)pcbSerializado);
	free(pcbSerializado);
}

void *hilo_PCP(void *arg) {
	t_proceso *proceso; int sock;

	while (1) {
		sem_wait(&sem_ready); sem_wait(&sem_cpu);
		//TODO:mutex
		sock = (int)queue_pop(cola_CPU_libres);
		proceso = queue_pop(cola_ready);

		printf("NUCLEO: Saco proceso %d ready, mando a exec\n", proceso->pcb->pid);
		mandarAEjecutar(proceso, sock);
	}
}

t_proceso* crearPrograma(int sock) {
	t_proceso* procesoNuevo;
	t_pcb *pcb;
	pcb = malloc(sizeof(t_pcb));
	procesoNuevo = malloc(sizeof(t_proceso));
	procesoNuevo->pcb = pcb;
	procesoNuevo->pcb->pid = pidcounter;
	procesoNuevo->socket_CONSOLA = sock;
	pidcounter++;
	return procesoNuevo;
}

int mandarCodigoAUmc(char* codigo, int size, t_proceso *proceso) {
	pthread_mutex_lock(&umcm);
	int cuantasPaginasCodigo, estado, i;
	t_header header;
	int *indiceCodigo;
	int tamanoIndiceCodigo;
	t_medatada_program* metadata_program;
	metadata_program = metadata_desde_literal(codigo);
	printf("OLA\n");
	proceso->pcb->paginasDeCodigo = ceil((double)size / (double)config_nucleo->TAMPAG);
	proceso->pcb->sizeIndiceDeCodigo =  (metadata_program->instrucciones_size);
	proceso->pcb->indiceDeCodigo = malloc(proceso->pcb->sizeIndiceDeCodigo * 2 * sizeof(int));

	//Creamos el indice de codigo
	for (i = 0; i < metadata_program->instrucciones_size; i++) {
		printf("Instruccion %.*s\n",metadata_program->instrucciones_serializado[i].offset,codigo+metadata_program->instrucciones_serializado[i].start);
		proceso->pcb->indiceDeCodigo[i * 2] = metadata_program->instrucciones_serializado[i].start;
		proceso->pcb->indiceDeCodigo[i * 2 + 1] = metadata_program->instrucciones_serializado[i].offset;
	}
	proceso->pcb->sizeIndiceDeEtiquetas = metadata_program->etiquetas_size;
	proceso->pcb->indiceDeEtiquetas = malloc(proceso->pcb->sizeIndiceDeEtiquetas * sizeof(char));
	memcpy(proceso->pcb->indiceDeEtiquetas, metadata_program->etiquetas, proceso->pcb->sizeIndiceDeEtiquetas * sizeof(char));

	proceso->pcb->contextoActual = list_create();
	t_contexto *contextocero;
	contextocero = malloc(sizeof(t_contexto));

	contextocero->sizeVars = 0;
	contextocero->sizeArgs = 0;
	contextocero->pos = 0;
	list_add(proceso->pcb->contextoActual, (void*)contextocero);

	
	proceso->pcb->sizeContextoActual = 1;
	proceso->pcb->pc = 0;
	proceso->pcb->paginasDeMemoria=(int)ceil((double)config_nucleo->STACK_SIZE / (double)config_nucleo->TAMPAG);
	//TODO: REVISAR QUE HACA FREE DE METADATA Y TODO QUEDDE ASIGNADO
	metadata_destruir(metadata_program);

	char*paqueteUMC; paqueteUMC=malloc(size+3*sizeof(int));
	int temp =proceso->pcb->paginasDeCodigo+proceso->pcb->paginasDeMemoria;
	//printf("Cantidad de paingas %d\n",temp);

	memcpy(paqueteUMC,&(proceso->pcb->pid), sizeof(int));
	memcpy(paqueteUMC+sizeof(int), &temp, sizeof(int));
	memcpy(paqueteUMC+2*sizeof(int), &size, sizeof(int));
	memcpy(paqueteUMC+3*sizeof(int), codigo, size);
	printf("OLAl\n");
	enviar(umc, 4,(size+3*sizeof(int)),paqueteUMC );
	printf("OLAkk\n");
	//TODO: mutex a umc
	t_paquete* paquete; 
	paquete = recibir(umc);


	free(paqueteUMC);
	pthread_mutex_unlock(&umcm);
	return paquete->codigo_operacion;
}

int *pideSemaforo(char *semaforo, int semaforoSize) {
	int i;
	printf("NUCLEO: pide sem %s\n", semaforo);

	for (i = 0; i < strlen((char*)config_nucleo->SEM_IDS) / sizeof(char*); i++) {
		//TODO:aca esta funcando con el \n OJO
		//TODO: mutex confignucleo
		if (strcmp((char*)config_nucleo->SEM_IDS[i], semaforo) == 0) {
			if (config_nucleo->VALOR_SEM[i] == 0) {return &config_nucleo->VALOR_SEM[i];}
			config_nucleo->VALOR_SEM[i]--;
			return (&config_nucleo->VALOR_SEM[i]);
		}
	}
	printf("No encontre SEM id, exit\n");exit(0);
}

int *pideVariable(char *variable, int tamanio) {
	int i;
	printf("NUCLEO: pide variable %s\n", variable);
	for (i = 0; i < strlen((char*)config_nucleo->SHARED_VARS) / sizeof(char*); i++) {
		//TODO:aca esta funcando con el \n OJO
		//TODO: mutex confignucleo
		if (strcmp((char*)config_nucleo->SHARED_VARS[i], variable) == 0) {
			return &config_nucleo->VALOR_SHARED_VARS[i];
		}
	}
	printf("No encontre variable id, exit\n");exit(0);
}


void escribeVariable(char *variable, int tamanio) {//
	//TODO ME TIENEN QUE MANDAR primero el int y luego el  string
	int *valor = (int*)variable;
	variable += tamanio;
	int i;
	for (i = 0; i < strlen((char*)config_nucleo->SHARED_VARS) / sizeof(char*); i++) {
		//TODO:aca esta funcando con el \n OJO
		//TODO: mutex confignucleo
		if (strcmp((char*)config_nucleo->SHARED_VARS[i], variable) == 0) {
			memcpy(&config_nucleo->VALOR_SHARED_VARS[i], valor, sizeof(int));
			return;
		}
	}
	printf("No encontre VAR id, exit\n");exit(0);

}


void liberaSemaforo(char *semaforo, int semaforoSize) {
	int i; t_proceso *proceso;
	printf("NUCLEO: libera sem %s\n", semaforo);
	for (i = 0; i < strlen((char*)config_nucleo->SEM_IDS) / sizeof(char*); i++) {
		if (strcmp((char*)config_nucleo->SEM_IDS[i], semaforo) == 0) {
		//TODO:aca esta funcando con el \n OJO
			//TODO: mutex confignucleo
			config_nucleo->VALOR_SEM[i]++;
			if (proceso = queue_pop(colas_semaforos[i])) {
				config_nucleo->VALOR_SEM[i]--;
				queue_push(cola_ready, proceso);
				sem_post(&sem_ready);
			}
			return;
		}
	}
	printf("No encontre SEM id, exit\n");exit(0);
}


void  bloqueoSemaforoManager(t_proceso *proceso, char *semaforo, int semSize) {
	int i;
	printf("NUCLEO: mando proceso %d a BLOCK por SEM\n", proceso->pcb->pid);
	for (i = 0; i < strlen((char*)config_nucleo->SEM_IDS) / sizeof(char*); i++) {
		if (strcmp((char*)config_nucleo->SEM_IDS[i], semaforo) == 0) {
			//TODO:aca esta funcando con el \n OJO
			//Mocks config colas
			queue_push(colas_semaforos[i], proceso);
			return;

		}
	}
	printf("No encontre SEM id, exit\n");exit(0);
}

void bloqueoIoManager(t_proceso *proceso, char *ioString, int sizeString, int unidadesBloqueado) {
	int i; int b = 0;
	printf("NUCLEO: mando proceso %d a BLOCK por IO\n", proceso->pcb->pid);
	for (i = 0; i < strlen((char*)config_nucleo->IO_IDS) / sizeof(char*); i++) {
		if (strcmp((char*)config_nucleo->IO_IDS[i], ioString) == 0) {
			//TODO: ojo con el \n
			proceso->unidadesBloqueado = unidadesBloqueado;
			//TODO: mutex
			if (queue_size(colas_ios[i]) == 0) {
				queue_push(colas_ios[i], proceso);
				makeTimer(timers[i], config_nucleo->VALOR_IO[i] * unidadesBloqueado, 0); //2ms
				return;
			} else {

			}
			queue_push(colas_ios[i], proceso);
			return;
		}

	}
	printf("No encontre IO id, exit\n");exit(0);

}


//TODO: refactor, sacar  intervalMS
static int makeTimer( timer_t *timerID, int expireMS, int intervalMS )
{
	struct sigevent te;
	struct itimerspec its;
	struct sigaction sa;
	int sigNo = SIGRTMIN;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = analizarIO;
	sigemptyset(&sa.sa_mask);
	if (sigaction(sigNo, &sa, NULL) == -1) {
		perror("sigaction");
	}
	te.sigev_notify = SIGEV_SIGNAL;
	te.sigev_signo = sigNo;
	te.sigev_value.sival_ptr = timerID;
	timer_create(CLOCK_REALTIME, &te, timerID);
	its.it_interval.tv_sec = floor(intervalMS / 1000);
	its.it_interval.tv_nsec = intervalMS % 1000 * 1000000;
	its.it_value.tv_sec =  floor(expireMS / 1000);
	its.it_value.tv_nsec = expireMS % 1000 * 1000000;
	timer_settime(*timerID, 0, &its, NULL);
	return 1;
}



void analizarIO(int sig, siginfo_t *si, void *uc) {
	int *tidp;
	int i, io;
	for (i = 0; i < strlen((char*)config_nucleo->IO_SLEEP) / sizeof(char*); i++) {
		//TODO: mutex
		if (timers[i] == si->si_value.sival_ptr) {io = i;}
	}

	t_proceso *proceso;
	//TODO: mutex
	proceso = queue_pop(colas_ios[io]);
	printf("\x1b[32mNUCLEO: Saco proceso %d de Cola IO %d mando READY\n\x1b[0m", proceso->pcb->pid, io);
	queue_push(cola_ready, proceso);
	sem_post(&sem_ready);
	if (queue_size(colas_ios[io]) != 0) {
		proceso = (t_proceso*)list_get(colas_ios[io]->elements, queue_size(colas_ios[io]) - 1);
		makeTimer(timers[io], config_nucleo->VALOR_IO[io] * proceso->unidadesBloqueado, 0); //2ms
	}

}

/********************************************************************************
*********************************************************************************
*********************************************************************************
************************CONSOLA STUFF********************************************
*********************************************************************************
*********************************************************************************
*********************************************************************************
*/

void *hilo_CONEXION_CONSOLA(void *socket) {

	int estado;
	t_proceso* proceso;

	//TODO: handshake
	proceso = crearPrograma(*(int*)socket);
	printf("NUCLEO:Handshake valido consola, creando proceso %d\n", proceso->pcb->pid);

	while (1) {
		int estado;
		t_paquete* paquete; 
		paquete = recibir(*(int*)socket);
		switch (paquete->codigo_operacion) {
			case -1:
				printf("NUCLEO: consola desconecto \n");
				//TODO:borrar proceso del sistema
				free(socket);
				free(paquete);
				return;
			case 103:
				estado = mandarCodigoAUmc(paquete->data, paquete->tamanio, proceso);

				if(estado==-1){
					printf("NUCLEO: ERROR EN SWAP\n");exit(0);
				}

				//TODO: ver lo del flag -1 fracaso / esito 1

				pthread_mutex_lock(&mutex_cola_new);
				queue_push(cola_new, proceso);
				sem_post(&sem_new);
				pthread_mutex_unlock(&mutex_cola_new);
			break;
		}
		//liberar_paquete()
	}
	//TODO: free el int socket
}

void *hilo_HANDLER_CONEXIONES_CONSOLA(void *arg) {

	int servidorSocket, socketCliente;
	int *socketClienteTemp;
	//struct arg_struct *args; 
	socketconsola = socket_escucha("localhost", config_nucleo->PUERTO_PROG);
	listen(socketconsola, 1024);
	while (1) {
		socketCliente = aceptar_conexion(socketconsola);
		socketClienteTemp = malloc(sizeof(int));
		*socketClienteTemp = socketCliente;
		pthread_t thCONEXION_CONSOLA;
		pthread_create(&thCONEXION_CONSOLA, NULL, hilo_CONEXION_CONSOLA, (void *)socketClienteTemp);
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


//TODO: quantum pueede cambiar ojo, hacer
//TODO: mandar tamano stack.


void *hilo_CONEXION_CPU(void *socket) {
	t_proceso* proceso;t_paquete * elPaquete;

	//Handshake
	t_datos_kernel datos_kernel;

	datos_kernel.QUANTUM = config_nucleo->QUANTUM;
	datos_kernel.QUANTUM_SLEEP = config_nucleo->QUANTUM_SLEEP;
	datos_kernel.TAMPAG = config_nucleo->TAMPAG;

	//O que no me envie nada? timeouts?
	printf("HOLA\n");
	enviar(*(int*)socket, 301, sizeof(t_datos_kernel), &datos_kernel);
		printf("REHOLA\n");

	//TODO VER ESTO

	//elPaquete = recibir(*(int*)socket);
	//liberar_paquete(elPaquete);

	//if(!elPaquete->codigo_operacion==302){printf("Error en handshake CPU\n");exit(0);}

	//TODO: mutex
	queue_push(cola_CPU_libres, (void*)*(int*)socket);
	sem_post(&sem_cpu);
	t_pcb *temp;
	while (1) {

		elPaquete = recibir(*(int*)socket);
		//printf("CRASH2 %d %d\n", elPaquete->codigo_operacion, *(int*)socket);




		//TODO IMPLEMENTAR FUNCION DE CAMBIAR EL CONTEXTO DEL QUUANTUM

		//TODO 370: abortado -> con pcb y destruyo




		switch (elPaquete->codigo_operacion) {
		case 304:
			proceso = dameProceso(cola_exec,*(int*)socket);

			printf("****ANTES DE DES\n");
			temp = desserializarPCB(elPaquete->data);
			printf("NUCLEO: Recibi proceso %d por fin de quantum, encolando en cola ready\n", proceso->pcb->pid);

			destruirPCB(proceso->pcb);
			proceso->pcb = temp;
			//TODO: mutex
			queue_push(cola_ready, proceso);
			sem_post(&sem_ready);
			queue_push(cola_CPU_libres, (void*)*(int*)socket);
			sem_post(&sem_cpu);

			break;

		case 320: //
			proceso = dameProceso(cola_exec, *(int*)socket);
			printf("NUCLEO: Recibi proceso %d por fin de ejecucion, encolando en cola exit\n", proceso->pcb->pid);
			queue_push(cola_exit, proceso);
			queue_push(cola_CPU_libres, (void*)*(int*)socket);
			sem_post(&sem_cpu);
			enviar(umc, 6, sizeof(int), &proceso->pcb->pid);
			break;

		case 380:
			proceso = dameProceso(cola_exec, *(int*)socket);
			int tiempo;
			tiempo=((int*)(elPaquete->data))[0];

//0 que no se bloquea.
			//340
			pthread_mutex_lock(&mutex_config);
			bloqueoIoManager(proceso, elPaquete->data+4, 10, tiempo);
			pthread_mutex_unlock(&mutex_config);

			elPaquete = recibir(*(int*)socket);
			//t_pcb *temp;
			printf("ANTES DE DES\n");
			temp = desserializarPCB(elPaquete->data);
			destruirPCB(proceso->pcb);
			proceso->pcb = temp;


			//TOO NO HAY QUE HACER UN PUSH DE PROCESO?

			break;

		case 399:
			proceso = dameProceso(cola_exec, *(int*)socket);
			t_blocked *bloqueo;
			printf("ANTES DE DES\n");
			bloqueo = desserializarBLOQUEO(elPaquete->data);
			printf(" DE DES\n");
			destruirPCB(proceso->pcb);
			proceso->pcb = bloqueo->pcb;

			if (bloqueo->ioSize) {

			}
			if (bloqueo->semaforoSize) {
				pthread_mutex_lock(&mutex_config);
				bloqueoSemaforoManager(proceso, bloqueo->semaforo, bloqueo->semaforoSize);
				pthread_mutex_unlock(&mutex_config);
			}

			free(bloqueo->io);
			free(bloqueo->semaforo);
			free(bloqueo);

			queue_push(cola_CPU_libres, (void*)*(int*)socket);
			sem_post(&sem_cpu);
			break;

		case 341: //Pide semaforo
			proceso = dameProceso(cola_exec, *(int*)socket);
			queue_push(cola_exec, proceso);
			enviar(*(int*)socket, 342, sizeof(int), pideSemaforo(elPaquete->data, elPaquete->tamanio));

			break;

		case 343: //Libera semaforo
			proceso = dameProceso(cola_exec, *(int*)socket);
			queue_push(cola_exec, proceso);
			pthread_mutex_lock(&mutex_config);
			liberaSemaforo(elPaquete->data, elPaquete->tamanio);
			pthread_mutex_unlock(&mutex_config);
			break;

		case 350: //Escribe variable
			proceso = dameProceso(cola_exec, *(int*)socket);
			queue_push(cola_exec, proceso);
			pthread_mutex_lock(&mutex_config);
			escribeVariable(elPaquete->data, elPaquete->tamanio);
			pthread_mutex_unlock(&mutex_config);
			break;

		case 351: //Pide variable
			proceso = dameProceso(cola_exec, *(int*)socket);
			queue_push(cola_exec, proceso);
			pthread_mutex_lock(&mutex_config);
			enviar(*(int*)socket, 352, sizeof(int), pideVariable(elPaquete->data, elPaquete->tamanio));
			pthread_mutex_unlock(&mutex_config);
			break;

		case 360: //imprimir
			proceso = dameProceso(cola_exec, *(int*)socket);
			queue_push(cola_exec, proceso);
			enviar(proceso->socket_CONSOLA, 160, elPaquete->tamanio, elPaquete->data);
			//int *a= elPaquete->data;
			printf("IMRPMMIR %d\n",*(int*)elPaquete->data);
		//	printf("IMRPMMIR %d\n",elPaquete->data);
		//	printf("IMRPMMIR %d\n",(int)*);
		case 361: //imprimir texto
			proceso = dameProceso(cola_exec, *(int*)socket);
			queue_push(cola_exec, proceso);
			enviar(proceso->socket_CONSOLA, 161, elPaquete->tamanio, elPaquete->data);
		}
		//AGREGAR RECIBO PCB POR FIN DE PROGAMA


	//TODO ver que mo rompa;		
//	liberar_paquete(elPaquete);
	}

}

void *hilo_HANDLER_CONEXIONES_CPU(void *arg) {
	int servidorSocket, socketCliente;
	int *socketClienteTemp;
	socketcpu = socket_escucha("localhost", config_nucleo->PUERTO_CPU);
	listen(socketcpu, 1024);

	while (1) {
		socketCliente = aceptar_conexion(socketcpu);
		socketClienteTemp = malloc(sizeof(int));
		*socketClienteTemp = socketCliente;

		pthread_t thCONEXION_CPU;
		pthread_create(&thCONEXION_CPU, NULL, hilo_CONEXION_CPU, (void *)socketClienteTemp);

		printf("NUCLEO: Acepte nueva CPU %d\n", *socketClienteTemp);
	}

}

void intHandler(int dummy) {
	close(socketcpu);
	close(socketconsola);
	close(umc);
	printf("NUCLEO: CERRÓ\n");
	printf("cierro Todo...\n\n");
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
	pthread_t thmock_consola, thmock_consola2, thmock_consola3, thmock_cpu;
	//	pthread_create(&thmock_cpu, NULL, hilo_mock_cpu, NULL);

	pthread_create(&thmock_consola, NULL, hilo_mock_consola, NULL);
//	pthread_create(&thmock_consola2, NULL, hilo_mock_consola, NULL);

	sleep(10);
	printf("HILOMOCK: envio");
	//pthread_create(&thmock_consola3, NULL, hilo_mock_consola, NULL);

while(1){}
	umcMock = servidor(1207);
	listen(umcMock, 5);
//	clienteUmc = accept(umcMock, (struct sockaddr *) &addr, &addrlen);

	printf("\x1b[31mUMCMOCK: Acepte conexion%d\n\x1b[0m", clienteUmc);

	t_paquete * paquete_nuevo;

	while (1) {
		paquete_nuevo = recibir(clienteUmc);
		printf("\x1b[31mUMCMOCK: recibi algo%d\n\x1b[0m\n", clienteUmc);
		if (paquete_nuevo->codigo_operacion == 204) {

		}

	}
}

void *hilo_mock_consola(void *arg) {
	int consola;
	sleep(4);
	consola = cliente("127.0.0.1", 1210);
	printf("\x1b[36mCONSOLAMOCK: Conecté%d \n\x1b[0m", consola);
	//codigo[0]='a'+rodo;
	//rodo++;
	enviar(consola, 103, sizeof(codigo), codigo);
	sleep(5);
	//close(consola);

	while (1) {}
}


void *hilo_mock_cpu(void *arg) {
	int cpu, estado;
	char var[8]  = "!Global\0";
	char sem[5]  = "SEM1\0";
	char ss[7]  = "Disco\0";
	char ee[8]  = "Scanner\0";
	sleep(5);
	cpu = cliente("127.0.0.1", 1202);
	printf("\x1b[31mCPUMOCK: Conecté%d\n\x1b[0m", cpu);

	t_blocked *bloqueado, *bloqueadoSerializado;

	t_paquete * paquete_nuevo;

	t_pcb *pcb;
	int i = 0;

	paquete_nuevo = recibir(cpu);
	enviar(cpu, 301, sizeof(codigo), codigo);

	while (1) {
		sleep(1);
		paquete_nuevo = recibir(cpu);
		

		if (paquete_nuevo->codigo_operacion == 303) {
			/*MODIFICAR Y PEDIR RECIBIR VARIABLE
			pcb = desserializarPCB(paquete_nuevo->data);
			printf("\x1b[31mCPUMOCK: Recibi pcb %d... ejecutando\n\x1b[0m", pcb->pid);

			char *mandar;
			mandar=malloc(sizeof(int)+sizeof(var));
			int a=3;
			memcpy(mandar, &a, sizeof(a));
			memcpy(mandar+sizeof(a), var, sizeof(var));


			enviar(cpu, 350, sizeof(int)+sizeof(var), mandar );
			enviar(cpu, 351, sizeof(var), var );
			*/

			//Test agregar contexto
			//t_contexto *contexto;
			//contexto = malloc(sizeof(t_contexto));
			//contexto->pos=pcb->sizeContextoActual;
			//agregarContexto(pcb,contexto);

			t_pcb * serializado;
			pcb = desserializarPCB(paquete_nuevo->data);
			serializado = (t_pcb*)serializarPCB(pcb);


			if(pcb->pid==1){
			enviar(cpu, 320, serializado->sizeTotal, serializado );
			printf("CPUMOCK: envie PCB a nucleo\n");

			}else{

			enviar(cpu, 304, serializado->sizeTotal, serializado );
			printf("CPUMOCK: envie PCB a nucleo\n");

			}



			/*
			//TEST IO

			if (i % 2 == 0) {

				bloqueado = malloc(sizeof(t_blocked));

				bloqueado->IO_time = 5;
				bloqueado->ioSize = sizeof(ss);
				bloqueado->io = ss;
				bloqueado->pcb = pcb;
				bloqueado->semaforoSize = 0;
				bloqueado->semaforo = NULL;

				bloqueadoSerializado = (t_blocked*)serializarBLOQUEO(bloqueado);


				enviar(cpu, 340, bloqueadoSerializado->sizeTotal, bloqueadoSerializado );

			}




			if (i % 2 != 0) {
				bloqueado = malloc(sizeof(t_blocked));

				bloqueado->IO_time = 5;
				bloqueado->ioSize = sizeof(ee);
				bloqueado->io = ee;
				bloqueado->pcb = pcb;
				bloqueado->semaforoSize = 0;
				bloqueado->semaforo = NULL;

				bloqueadoSerializado = (t_blocked*)serializarBLOQUEO(bloqueado);


				enviar(cpu, 340, bloqueadoSerializado->sizeTotal, bloqueadoSerializado );
			}

			printf("\x1b[31mCPUMOCK: envie proceso a bloqueo disco\x1b[0m\n");
			free(bloqueadoSerializado);
			free(bloqueado);

			i++;

			*/

			/*
			test bloqueado
			if (i%3==0){
				enviar(cpu, 341, sizeof(sem), sem );

			}
			if (i%3== 2 ) {
				enviar(cpu, 343, sizeof(sem), sem );
				t_pcb * serializado;
				serializado = (t_pcb*)serializarPCB(pcb);
				enviar(cpu, 304, serializado->sizeTotal, serializado );
				printf("CPUMOCK: envie PCB a nucleo\n");
			}

			*/
		}//End if paquete

		/* test bloqueado
		if (paquete_nuevo->codigo_operacion == 342) {


			if(i%3==1){
				bloqueado = malloc(sizeof(t_blocked));

				bloqueado->IO_time = 0;
				bloqueado->ioSize = 0;
				bloqueado->io = NULL;
				bloqueado->pcb = pcb;
				bloqueado->semaforoSize = sizeof(sem);
				bloqueado->semaforo = sem;

				bloqueadoSerializado = (t_blocked*)serializarBLOQUEO(bloqueado);

				printf("\x1b[31mCPUMOCK: envie proceso a bloqueo SEM\x1b[0m\n");
				enviar(cpu, 340, bloqueadoSerializado->sizeTotal, bloqueadoSerializado );
			}//FIN I
			}

			*/

		/* RECIBIR VARIABLE
		if (paquete_nuevo->codigo_operacion == 352) {
			int *a=(int*)paquete_nuevo->data;
			printf("\x1b[31mCPUMOCK: Recibi variable %d... \n\x1b[0m",*a );

		}

		*/

		i++;
		//printf("NUEVO I%d\n",i);
	}//FIN WHILE

} //FIN MOCK CPU









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
	//TODO:Mutex son generales, uso uno solo?
	pthread_mutex_lock(&total);
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
	config_nucleo->TAMPAG = config_get_int_value(fnucleo, "SIZE_PAGINA");
	config_nucleo->IP_UMC = config_get_string_value(fnucleo, "IP_UMC");
	config_nucleo->PUERTO_UMC = config_get_string_value(fnucleo, "PUERTO_UMC");
	config_nucleo->VALOR_SHARED_VARS = convertirConfig0(config_nucleo->SHARED_VARS);
	//Iniciar en 0
	//Hacer

	config_nucleo->VALOR_IO = convertirConfigInt(config_nucleo->IO_SLEEP, config_nucleo->IO_IDS);
	config_nucleo->VALOR_SEM = convertirConfigInt(config_nucleo->SEM_INIT, config_nucleo->SEM_IDS);

	//config_nucleo->VALOR_IO_EXPIRED_TIME = punteroConCero(config_nucleo->IO_IDS);

	//Crear colar IO
	//t_queue** colas_IO;
	timers = malloc(strlen((char*)config_nucleo->IO_SLEEP) * sizeof(char*));

	colas_ios = malloc(strlen((char*)config_nucleo->IO_SLEEP) * sizeof(char*));

	int i;
	for (i = 0; i < strlen((char*)config_nucleo->IO_SLEEP); i++) {

		timers[i] = malloc(sizeof(timer_t));
		colas_ios[i] = malloc(sizeof(t_queue*));
		colas_ios[i] = queue_create();
	}

	colas_semaforos = malloc(strlen((char*)config_nucleo->SEM_INIT) * sizeof(char*));

	for (i = 0; i < strlen((char*)config_nucleo->SEM_INIT); i++) {

		colas_semaforos[i] = malloc(sizeof(t_queue*));
		colas_semaforos[i] = queue_create();
	}

	pthread_mutex_unlock(&total);

	//config_destroy(fnucleo);//Ya no lo necesito
	return;
}
int *convertirConfigInt(char **ana1, char **ana2) {
	int i;
	int *resul;
	resul = malloc(((strlen((char*)ana1)) / sizeof(char*)) * sizeof(int));
	for (i = 0; i < (strlen((char*)ana1)) / sizeof(char*); i++) {
		resul[i] = atoi(ana1[i]);
	}
	return resul;
}
int *convertirConfig0(char **ana1) {
	int i;
	int *resul;
	resul = malloc(((strlen((char*)ana1)) / sizeof(char*)) * sizeof(int));
	for (i = 0; i < (strlen((char*)ana1)) / sizeof(char*); i++) {
		resul[i] = 0;
	}
	return resul;
}

/* eliminar
long long current_timestamp(void) {
	struct timeval te;
	gettimeofday(&te, NULL);
	long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000;
	return milliseconds;
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
void printbuf(const char* buffer, int len) {
	int i;
	printf("\x1b[31m INICIO PRINT\n");
	for ( i = 0; i < len; ++i)
		if (buffer[i] >= 48 && buffer[i] <= 122) printf("%c", buffer[i]);
		else  printf("*");

	printf("\nFIN PRINT\n\x1b[0m");
}

*/
