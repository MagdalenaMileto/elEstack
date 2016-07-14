/*
 * nucleo.c
 *
 *  Created on: 23/4/2016
 *      Author: nico
 */

#include "nucleo.h"
#define CONFIG_NUCLEO_SIMPLE "nucleo.config" //Cambiar esto para eclipse
#define CONFIG_NUCLEO  "configuracion/nucleo.config"
#define CONFIG_PATH "configuracion/"

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
t_log * logger;
//semaforos
sem_t sem_ready;
sem_t sem_cpu;
sem_t sem_new;

timer_t *timerPantalla;

//Colas sem
t_queue** colas_semaforos;

pthread_mutex_t mutex_cola_new, mutex_cola_ready, mutex_variables,total,umcm;


pthread_mutex_t mcola_new,mcola_exit,mcola_exec,mcola_ready,mcolas_io,mcolas_sem,mcola_CPU_libres;


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

char codigo[250] = "#!/usr/bin/ansisop\nbegin\n:etiqueta\ntextPrint    Hola Mundo!\ngoto etiqueta\nend";

/* FIN DE VARIABLES GLOBALES */

void gotoxy(int x,int y)
{
printf("%c[%d;%df",0x1B,y,x);
}

int main() {
	signal(SIGINT, intHandler);
	logger = log_create("NUCLEO.log", "NUCLEO", 0, LOG_LEVEL_INFO);


	log_info(logger, "*************************************************");
	log_info(logger, "*************NUCLEO INICIO***********************");
	log_info(logger, "*************************************************");



	//Levantar archivo de configuracion

	timerPantalla=nalloc(sizeof(timer_t));


	get_config_nucleo();//Crea y setea el config del kernel



	

	//MOCK SACAR UNA VEZ INTEGRADO
	pthread_t mock;

	//	pthread_create(&mock, NULL, hilo_mock, NULL);
	//	sleep(3);

	//Inicializaciones 

	sem_init(&sem_cpu, 0, 0); // Los uso

	sem_init(&sem_new, 0, 0);
	sem_init(&sem_ready, 0, 0);





	cola_new = queue_create();
	cola_exec = queue_create();
	cola_ready = queue_create();
	//printf("COLA SIEZE%d\n",)
	cola_block = queue_create();
	cola_exit = queue_create();
	cola_CPU_libres = queue_create();

	pthread_mutex_init(&umcm, NULL);
	pthread_mutex_init(&mutex_cola_new, NULL);
	pthread_mutex_init(&total, NULL);


	pthread_mutex_init(&mutex_config, NULL);
	pthread_mutex_init(&mcola_new, NULL);
	pthread_mutex_init(&mcola_exit, NULL);
	pthread_mutex_init(&mcola_ready, NULL);
	pthread_mutex_init(&mcolas_sem, NULL);
	pthread_mutex_init(&mcolas_io, NULL);


	pthread_t thPCP, thPLP, thCONEXIONES_CPU, thCONEXIONES_CONSOLA;
	pthread_create(&thCONEXIONES_CONSOLA, NULL, hilo_HANDLER_CONEXIONES_CONSOLA, NULL);
	pthread_create(&thCONEXIONES_CPU, NULL, hilo_HANDLER_CONEXIONES_CPU, NULL);
	pthread_create(&thPCP, NULL, hilo_PCP, NULL);
	pthread_create(&thPLP, NULL, hilo_PLP, NULL);
	makeTimer(timerPantalla, 300, 300); //2ms

	conectarUmc();

	//INOTIFY STUFF
	int file_descriptor = inotify_init();
	if (file_descriptor < 0) perror("inotify_init");
	int watch_descriptor = inotify_add_watch(file_descriptor, CONFIG_PATH , IN_CLOSE_WRITE);
	char buffer[1000];
	//while(1){}
	while(1){
		int length = read(file_descriptor, buffer, 1000);
		if (length < 0) {
			//perror("read");
		}
		int offset = 0;
		while (offset < length) {
			struct inotify_event *event = (struct inotify_event *) &buffer[offset];
			if (event->len) {
				if (event->mask & IN_CLOSE_WRITE) {
					if (!(event->mask & IN_ISDIR)) {

					//CAMBIAR ESTO PARA ENTREGA FINAL
					//Deberia hacer un free de todo lo otro
						//printf("EVENTO\n");
						if (strcmp(event->name, CONFIG_NUCLEO_SIMPLE) == 0){
							log_info(logger, "NUCLEO: cambio el archivo config");
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
	t_paquete* paquete;
	paquete = recibir(umc);
	config_nucleo->TAMPAG=*((int*)paquete->data);
	log_info(logger, "NUCLEO: Tamano pagina del UMC %d",config_nucleo->TAMPAG);

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
		pthread_mutex_lock(&mcola_new);
		proceso = queue_pop(cola_new);
		pthread_mutex_unlock(&mcola_new);
		log_info(logger, "\x1b[3%dmNUCLEO: saco proceso %d new mando a ready\x1b[0m", ((proceso->pcb->pid)%6+1),proceso->pcb->pid);
		//printf();

		pthread_mutex_lock(&mcola_ready);
		queue_push(cola_ready, proceso);
		pthread_mutex_unlock(&mcola_ready);

		sem_post(&sem_ready);
	}
}

void mandarAEjecutar(t_proceso *proceso, int sock) {
	t_pcb *pcbSerializado;

	pcbSerializado = (t_pcb*)serializarPCB(proceso->pcb);

	proceso->socket_CPU = sock;
	//TODO:mutex
	pthread_mutex_lock(&mcola_exec);
	queue_push(cola_exec, proceso);
	pthread_mutex_unlock(&mcola_exec);

	t_datos_kernel datos_kernel;
	datos_kernel.QUANTUM = config_nucleo->QUANTUM;
	datos_kernel.QUANTUM_SLEEP = config_nucleo->QUANTUM_SLEEP;
	datos_kernel.TAMPAG = config_nucleo->TAMPAG;
	datos_kernel.STACK_SIZE = config_nucleo->STACK_SIZE;

	//O que no me envie nada? timeouts?

	enviar(sock, 301, sizeof(t_datos_kernel), &datos_kernel);
	//usleep(50000);

	enviar(sock, 303, pcbSerializado->sizeTotal, (char*)pcbSerializado);
//	printf("NUCLEO:mande a ejecutar PID%d\n",pcbSerializado->pid);

	//free(pcbSerializado);
}

void *hilo_PCP(void *arg) {
	t_proceso *proceso; int sock;

	while (1) {
		 sem_wait(&sem_cpu);sem_wait(&sem_ready);
		//TODO:mutex
		//printf("Soy casi grande\n");
		pthread_mutex_lock(&mutex_config);
		sock = (int)queue_pop(cola_CPU_libres);
		pthread_mutex_lock(&mcola_ready);
		proceso = queue_pop(cola_ready);
		pthread_mutex_unlock(&mcola_ready);
		log_info(logger,"\x1b[3%dmNUCLEO: saco proceso %d ready mando a ejecutar\x1b[0m", ((proceso->pcb->pid)%6+1),proceso->pcb->pid);


		mandarAEjecutar(proceso, sock);
		pthread_mutex_unlock(&mutex_config);

	}
}

t_proceso* crearPrograma(int sock) {
	t_proceso* procesoNuevo;
	t_pcb *pcb;
	pcb = nalloc(sizeof(t_pcb));
	procesoNuevo = nalloc(sizeof(t_proceso));
	procesoNuevo->pcb = pcb;
	procesoNuevo->pcb->pid = pidcounter;
	procesoNuevo->socket_CONSOLA = sock;
	procesoNuevo->abortado=false;
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

	proceso->pcb->paginasDeCodigo = ceil((double)size / (double)config_nucleo->TAMPAG);
	proceso->pcb->sizeIndiceDeCodigo =  (metadata_program->instrucciones_size);
	proceso->pcb->indiceDeCodigo = malloc(proceso->pcb->sizeIndiceDeCodigo * 2 * sizeof(int));

	//Creamos el indice de codigo
	for (i = 0; i < metadata_program->instrucciones_size; i++) {
		log_info(logger,"Instruccion inicio:%d offset:%d %.*s",metadata_program->instrucciones_serializado[i].start,metadata_program->instrucciones_serializado[i].offset,metadata_program->instrucciones_serializado[i].offset,codigo+metadata_program->instrucciones_serializado[i].start);
		proceso->pcb->indiceDeCodigo[i * 2] = metadata_program->instrucciones_serializado[i].start;
		proceso->pcb->indiceDeCodigo[i * 2 + 1] = metadata_program->instrucciones_serializado[i].offset;
	}
	proceso->pcb->sizeIndiceDeEtiquetas = metadata_program->etiquetas_size;
	proceso->pcb->indiceDeEtiquetas = malloc(proceso->pcb->sizeIndiceDeEtiquetas * sizeof(char));
	memcpy(proceso->pcb->indiceDeEtiquetas, metadata_program->etiquetas, proceso->pcb->sizeIndiceDeEtiquetas * sizeof(char));

	proceso->pcb->contextoActual = list_create();

	t_contexto *contextocero;
	contextocero = malloc(sizeof(t_contexto));

	contextocero->args = list_create();
	contextocero->vars = list_create();

	contextocero->sizeVars = 0;
	contextocero->sizeArgs = 0;
	contextocero->pos = 0;
	list_add(proceso->pcb->contextoActual, (void*)contextocero);

	
	proceso->pcb->sizeContextoActual = 1;
	proceso->pcb->pc = 0;
	(proceso->pcb)->paginasDeMemoria=(int)ceil((double)config_nucleo->STACK_SIZE );
	//TODO: REVISAR QUE HACA FREE DE METADATA Y TODO QUEDDE ASIGNADO
	metadata_destruir(metadata_program);

	char*paqueteUMC; paqueteUMC=malloc(size+3*sizeof(int));
	int temp =proceso->pcb->paginasDeCodigo+proceso->pcb->paginasDeMemoria;
	log_info(logger,"NUCLEO: Cantidad de paingas %d",temp);

	memcpy(paqueteUMC,&(proceso->pcb->pid), sizeof(int));
	memcpy(paqueteUMC+sizeof(int), &temp, sizeof(int));
	memcpy(paqueteUMC+2*sizeof(int), &size, sizeof(int));
	memcpy(paqueteUMC+3*sizeof(int), codigo, size);

	enviar(umc, 4,(size+3*sizeof(int)),paqueteUMC );

	//TODO: mutex a umc
	t_paquete* paquete; 
	paquete = recibir(umc);
	int retorno = paquete->codigo_operacion;

//	free(paqueteUMC);
	liberar_paquete(paquete);
	pthread_mutex_unlock(&umcm);
	return retorno;
}

int *pideSemaforo(char *semaforo) {
	int i;
	//printf("NUCLEO: pide sem %s\n", semaforo);

	for (i = 0; i < strlen((char*)config_nucleo->SEM_IDS) / sizeof(char*); i++) {
		//TODO: mutex confignucleo
		if (strcmp((char*)config_nucleo->SEM_IDS[i], semaforo) == 0) {
			
			//if (config_nucleo->VALOR_SEM[i] == -1) {return &config_nucleo->VALOR_SEM[i];}
			//config_nucleo->VALOR_SEM[i]--;
			return (&config_nucleo->VALOR_SEM[i]);
		}
	}
	printf("No encontre SEM id, exit\n");exit(0);
}

void escribeSemaforo(char *semaforo,int valor){
	int i;
	//printf("NUCLEO: pide sem %s\n", semaforo);

	for (i = 0; i < strlen((char*)config_nucleo->SEM_IDS) / sizeof(char*); i++) {
		//TODO: mutex confignucleo
		if (strcmp((char*)config_nucleo->SEM_IDS[i], semaforo) == 0) {
			
			//if (config_nucleo->VALOR_SEM[i] == -1) {return &config_nucleo->VALOR_SEM[i];}
			config_nucleo->VALOR_SEM[i]=valor;
			return;
		}
	}
	printf("No encontre SEM id, exit\n");exit(0);
}

int *pideVariable(char *variable) {
	int i;
	log_info(logger,"NUCLEO: pide variable %s", variable);
	for (i = 0; i < strlen((char*)config_nucleo->SHARED_VARS) / sizeof(char*); i++) {
		//TODO:aca esta funcando con el \n OJO
		//TODO: mutex confignucleo
		if (strcmp((char*)config_nucleo->SHARED_VARS[i], variable) == 0) {
			return &config_nucleo->VALOR_SHARED_VARS[i];
		}
	}
	printf("No encontre variable %s %d id, exit\n",variable,strlen(variable));exit(0);
}


void escribeVariable(char *variable) {//
	//TODO ME TIENEN QUE MANDAR primero el int y luego el  string
	int *valor = (int*)variable;
	variable += 4;
	int i;
	for (i = 0; i < strlen((char*)config_nucleo->SHARED_VARS) / sizeof(char*); i++) {
		//TODO:aca esta funcando con el \n OJO
		//TODO: mutex confignucleo
		if (strcmp((char*)config_nucleo->SHARED_VARS[i], variable) == 0) {
			//printf("ASIGNO%d\n",*valor);
			memcpy(&config_nucleo->VALOR_SHARED_VARS[i], valor, sizeof(int));
			return;
		}
	}
	printf("No encontre VAR %s id, exit\n",variable);exit(0);

}


void liberaSemaforo(char *semaforo) {
	int i; t_proceso *proceso;

	for (i = 0; i < strlen((char*)config_nucleo->SEM_IDS) / sizeof(char*); i++) {
		if (strcmp((char*)config_nucleo->SEM_IDS[i], semaforo) == 0) {
		
			if(list_size(colas_semaforos[i]->elements)){
				proceso = queue_pop(colas_semaforos[i]);
				pthread_mutex_lock(&mcola_ready);
				queue_push(cola_ready, proceso);
				pthread_mutex_unlock(&mcola_ready);
				sem_post(&sem_ready);
			}else{
				config_nucleo->VALOR_SEM[i]++;
			}

			return;
/*


		//TODO:aca esta funcando con el \n OJO
			//TODO: mutex confignucleo
			config_nucleo->VALOR_SEM[i]++;
			printf("VALRO SEM %d\n",config_nucleo->VALOR_SEM[i]);
			if (proceso = queue_pop(colas_semaforos[i])) {
				//config_nucleo->VALOR_SEM[i]--;
				queue_push(cola_ready, proceso);
				sem_post(&sem_ready);
			}
			return;

			*/
		}
	}
	printf("No encontre SEM id, exit\n");exit(0);
}


void  bloqueoSemaforo(t_proceso *proceso, char *semaforo) {
	int i;

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

void bloqueoIoManager(t_proceso *proceso, char *ioString, int unidadesBloqueado) {
	int i; int b = 0;
	//printf("NUCLEO: mando  %s proceso %d a BLOCK por IO\n",ioString, proceso->pcb->pid);
	for (i = 0; i < strlen((char*)config_nucleo->IO_IDS) / sizeof(char*); i++) {
		if (strcmp((char*)config_nucleo->IO_IDS[i], ioString) == 0) {
			//TODO: ojo con el \n
			proceso->unidadesBloqueado = unidadesBloqueado;
			//TODO: mutex
			if (queue_size(colas_ios[i]) == 0) {
				queue_push(colas_ios[i], proceso);

				//printf("LA SIZE: %d",queue_size(colas_ios[i]));
				makeTimer(timers[i], config_nucleo->VALOR_IO[i] * unidadesBloqueado, 0); //2ms
				return;
			} else {

			}
			queue_push(colas_ios[i], proceso);
			//printf("LA SIZE: %d\n",queue_size(colas_ios[i]));
			return;
		}

	}
	printf("No encontre IO id x, exit\n");exit(0);

}


//TODO: refactor, sacar  intervalMS
static int makeTimer( timer_t *timerID, int expireMS, int intervalMS )
{
	struct sigevent te;
	struct itimerspec its;
	struct sigaction sa;
	int sigNo = SIGRTMIN;
	sa.sa_flags = SA_SIGINFO|SA_RESTART;

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
	if (si->si_value.sival_ptr==timerPantalla){dibujarPantalla();return;}
	for (i = 0; i < strlen((char*)config_nucleo->IO_SLEEP) / sizeof(char*); i++) {
		//TODO: mutex
		if (timers[i] == si->si_value.sival_ptr) {io = i;}
	}
	//printf("deberia entrar una vez\n");
	t_proceso *proceso;
	//TODO: mutex
	proceso = queue_pop(colas_ios[io]);
	if(proceso->abortado==true){
		log_info(logger,"\x1b[32mNUCLEO: Saco proceso %d de Cola IO %d por aborto consola\x1b[0m", proceso->pcb->pid, io);
		abortar(proceso);
	}else{
		log_info(logger,"\x1b[32mNUCLEO: Saco proceso %d de Cola IO %d mando READY\x1b[0m", proceso->pcb->pid, io);
		pthread_mutex_lock(&mcola_ready);
	queue_push(cola_ready, proceso);
	pthread_mutex_unlock(&mcola_ready);
	sem_post(&sem_ready);
	}
	if (queue_size(colas_ios[io]) != 0) {
		//printf("todavi\n");
		proceso = (t_proceso*)list_get(colas_ios[io]->elements, queue_size(colas_ios[io]) - 1);
		makeTimer(timers[io], config_nucleo->VALOR_IO[io] * proceso->unidadesBloqueado, 0); //2ms
	}

}


void dibujarPantalla(void){
	//printf("HOLIS\n");
	printf("\e[1;1H\e[2J");printf("\e[?25l");
	t_proceso *proceso;
	int i;int a=1;int x;

	gotoxy(30,5);
	printf("║Cola New");
	pthread_mutex_lock(&mcola_new);
	for(x=0;x<list_size(cola_new->elements);x++){
		gotoxy(28-3*x,5);
		proceso = list_get(cola_new->elements,x);
		printf("\x1b[3%dmP%d\x1b[0m",((proceso->pcb->pid)%6+1),proceso->pcb->pid);
	}
	pthread_mutex_unlock(&mcola_new);


	gotoxy(30,6);
	printf("║Cola Ready");
	pthread_mutex_lock(&mcola_ready);
	for(x=0;x<list_size(cola_ready->elements);x++){
		gotoxy(28-3*x,6);
		proceso = list_get(cola_ready->elements,x);

		printf("\x1b[3%dmP%d\x1b[0m",((proceso->pcb->pid)%6+1),proceso->pcb->pid);
	}
	pthread_mutex_unlock(&mcola_ready);


	gotoxy(30,7);
	printf("║Cola Exec");
	pthread_mutex_lock(&mcola_exec);
	for(x=0;x<list_size(cola_exec->elements);x++){
		gotoxy(28-3*x,7);

		proceso = list_get(cola_exec->elements,x);
		printf("\x1b[3%dmP%d\x1b[0m",((proceso->pcb->pid)%6+1),proceso->pcb->pid);
	}
	pthread_mutex_unlock(&mcola_exec);


	//log_info(logger, "\x1b[3%dmNUCLEO: saco proceso %d new mando a ready\x1b[0m\n", );

	//config_nucleo->VALOR_IO = convertirConfigInt(config_nucleo->IO_SLEEP, config_nucleo->IO_IDS);
	//config_nucleo->VALOR_SEM = convertirConfigInt(config_nucleo->SEM_INIT, config_nucleo->SEM_IDS);
//printf("aaa\n");

	for(i=0;i<strlen((char*)config_nucleo->IO_IDS)/ sizeof(char*);i++){
		gotoxy(30,7+a);
		printf("║Cola IO %s\n",(char*)config_nucleo->IO_IDS[i]);

		for(x=0;x<list_size(colas_ios[i]->elements);x++){
			gotoxy(28-3*x,7+a);
			proceso = list_get(colas_ios[i]->elements,x);
			printf("\x1b[3%dmP%d\x1b[0m",((proceso->pcb->pid)%6+1),proceso->pcb->pid);

		}

		a++;
		//if(list_size(colas_ios[i]->elements)>0) retorno = true;
	}
	for(i=0;i<strlen((char*)config_nucleo->SEM_IDS)/ sizeof(char*);i++){
		gotoxy(30,7+a);
		printf("║Cola SEM %s   Valor Actual: %d\n",(char*)config_nucleo->SEM_IDS[i],config_nucleo->VALOR_SEM[i]);
		for(x=0;x<list_size(colas_semaforos[i]->elements);x++){
			gotoxy(28-3*x,7+a);
			proceso = list_get(colas_semaforos[i]->elements,x);
			printf("\x1b[3%dmP%d\x1b[0m",((proceso->pcb->pid)%6+1),proceso->pcb->pid);

		}
		a++;
	}

	gotoxy(30,7+a);
	printf("║Cola EXIT");
	pthread_mutex_lock(&mcola_exit);
	for(x=0;x<list_size(cola_exit->elements);x++){
		gotoxy(28-3*x,7+a);
		proceso = list_get(cola_exit->elements,x);
		printf("\x1b[3%dmP%d\x1b[0m",((proceso->pcb->pid)%6+1),proceso->pcb->pid);
	}
	pthread_mutex_unlock(&mcola_exit);


	//colas_ios[i]->elements
	fflush(stdout);

}
/********************************************************************************
*********************************************************************************
*********************************************************************************
************************CONSOLA STUFF********************************************
*********************************************************************************
*********************************************************************************
*********************************************************************************
*/
void abortar(t_proceso *proceso){
	pthread_mutex_lock(&mcola_exit);
	destruirCONTEXTO(proceso->pcb);
	queue_push(cola_exit, proceso);
	pthread_mutex_unlock(&mcola_exit);
	pthread_mutex_lock(&umcm);
	enviar(umc, 6, sizeof(int), &proceso->pcb->pid);
	pthread_mutex_unlock(&umcm);
}
void abortarProceso(int SockId){

	t_proceso *procesin;

			bool esMiConsola(void * entrada) {
				t_proceso * proceso = (t_proceso *) entrada;
				return proceso->socket_CONSOLA==SockId;
			}

			pthread_mutex_lock(&mcola_exit);
			procesin=(t_proceso*)list_find(cola_exit->elements,esMiConsola);
			pthread_mutex_unlock(&mcola_exit);
			if(procesin!=NULL){
				log_info(logger,"Ya estaba en exit");
				return;
			}



			//printf("TAMANIO %d\n",list_size(cola_ready->elements));
			pthread_mutex_lock(&mcola_ready);
			procesin=(t_proceso*)list_remove_by_condition(cola_ready->elements,esMiConsola);
			pthread_mutex_unlock(&mcola_ready);
			//printf("LA LISTA QUEDO EN %d\n",list_size(cola_ready->elements));

			if(procesin!=NULL){
				//Caso que lo tengo en ready
				log_info(logger,"NUCLEO: Abortado x consola, en ready");
				//printf("entra\n");
				if(list_size(cola_CPU_libres->elements)!=0) sem_wait(&sem_cpu);
				if(list_size(cola_ready->elements)!=0) sem_wait(&sem_ready);
				//printf("sale\n");
				abortar(procesin);
			}else{
				//Aca no lo tengo en ready, lo busco en exec
				pthread_mutex_lock(&mcola_exec);
				procesin=(t_proceso*)list_find(cola_exec->elements,esMiConsola);
				pthread_mutex_unlock(&mcola_exec);

				if(procesin!=NULL){
					log_info(logger,"NUCLEO: Abortado x consola, en exec, espero que termine de trabajar");
					procesin->abortado=true;

				}else{
					//no hay en exec, lo busco en semaforos
					int i;
					for (i = 0; i < strlen((char*)config_nucleo->SEM_IDS) / sizeof(char*); i++) {

						procesin=(t_proceso*)list_remove_by_condition(colas_semaforos[i]->elements,esMiConsola);
						if(procesin!=NULL) break;

					}
					if(procesin!=NULL){
						log_info(logger,"NUCLEO: Abortado x consola, en semaforo");
						abortar(procesin);

					}else{
						//Busco en IO

						for (i = 0; i < strlen((char*)config_nucleo->IO_IDS) / sizeof(char*); i++) {

							procesin=list_get(colas_ios[i]->elements,0);
							if(procesin!=NULL){
								if(procesin->socket_CONSOLA==SockId){
									log_info(logger,"NUCLEO: Abortado x consola, en IO, libero");
									procesin->abortado=true;
									makeTimer(timers[i], 1, 0); //2ms

								//Esta ejecutando
								}
							}
								//sleep(1);
								log_info(logger,"TAMANOI %d:",queue_size(colas_ios[i]));
							procesin=(t_proceso*)list_remove_by_condition(colas_ios[i]->elements,esMiConsola);
							if(procesin!=NULL) {
								log_info(logger,"\x1b[32mNUCLEO: Saco proceso %d de Cola IO %d mando exit. no procesado\x1b[0m", procesin->pcb->pid, i);

								abortar(procesin);
							}else{
								abortar(procesin);
								log_info(logger,"\x1b[32mNUCLEO: ***** Saco proceso %d de ?? %d mando exit. \x1b[0m", procesin->pcb->pid, i);

								//printf("este caso no se podria dar\n");
							}



						}//Cierre del for


						}


				}//FIN SEMAFOROS


				}//Fin exec

			} // fin del ready








void *hilo_CONEXION_CONSOLA(void *socket) {

	int estado;
	t_proceso* proceso;

	//TODO: handshake
	proceso = crearPrograma(*(int*)socket);
	log_info(logger,"NUCLEO: Handshake valido consola, creando proceso %d", proceso->pcb->pid);

	while (1) {
		int estado;
		t_paquete* paquete; 
		paquete = recibir(*(int*)socket);
		switch (paquete->codigo_operacion) {
			case -1:
				log_info(logger,"NUCLEO: consola desconecto  ");
				abortarProceso(*(int*)socket);
				//TODO:borrar proceso del sistema
				free(socket);
				free(paquete);
				return 0;
			case 103:
				estado = mandarCodigoAUmc(paquete->data, paquete->tamanio, proceso);

				if(estado==-1){
					log_info(logger,"NUCLEO: ERROR EN SWAP");
					enviar(*(int*)socket, 163, sizeof(int), &estado);//
					pthread_mutex_lock(&umcm);
					enviar(umc, 6, sizeof(int), &proceso->pcb->pid);
					pthread_mutex_unlock(&umcm);

					return 0;
					//exit(0);
				}

				//TODO: ver lo del flag -1 fracaso / esito 1

				pthread_mutex_lock(&mutex_cola_new);
				queue_push(cola_new, proceso);
				sem_post(&sem_new);
				pthread_mutex_unlock(&mutex_cola_new);
			break;
		}
		liberar_paquete(paquete);
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
		log_info(logger,"NUCLEO: Acepte consola %d", socketCliente);
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
	t_paquete * elPaquete2;

	//Handshake
	t_datos_kernel datos_kernel;
	//TODO remover esto que ya lo hacer en otro lado
	datos_kernel.QUANTUM = config_nucleo->QUANTUM;
	datos_kernel.QUANTUM_SLEEP = config_nucleo->QUANTUM_SLEEP;
	datos_kernel.TAMPAG = config_nucleo->TAMPAG;
	datos_kernel.STACK_SIZE = config_nucleo->STACK_SIZE;

	//O que no me envie nada? timeouts?

	enviar(*(int*)socket, 301, sizeof(t_datos_kernel), &datos_kernel);

	log_info(logger,"NUCLEO: conecto nueva CPU");

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
		//printf("NUCLEO COD OPERACION: %d %d\n", elPaquete->codigo_operacion, *(int*)socket);



		switch (elPaquete->codigo_operacion) {

		case -1:
			log_info(logger,"NUCLEO: ABORTO CPU, TERMINAR %d",list_size(cola_exec->elements));
			int a = 0;int w;
			t_proceso * procesin;

			bool hayProcesoAbortado(void* entrada) {
				log_info(logger,"pase x aca");
				t_proceso * proceso = (t_proceso *) entrada;
				return (proceso->socket_CPU)==*(int*)socket;
			}


			procesin=(t_proceso*)list_remove_by_condition(cola_exec->elements,hayProcesoAbortado);
			if(procesin!=NULL){
			log_info(logger,"NUCLEO: ABORTO CPU, SACO COLA CPU READY. HABIA PROCESO EXEC.");
			abortar(procesin);
			}else{
			log_info(logger,"NUCLEO: ABORTO CPU, SACO COLA CPU READY. NO HABIA PROCESO EXEC.");
			}

			while (w = (int)list_get(cola_CPU_libres->elements, a)) {
				if (w == *(int*)socket) {list_remove(cola_CPU_libres->elements, a);
				sem_wait(&sem_cpu);
				}
				a++;
			}
			return 0;
			break;
		case 304:
			pthread_mutex_lock(&mcola_exec);
			proceso = dameProceso(cola_exec,*(int*)socket);
			pthread_mutex_unlock(&mcola_exec);
			temp = desserializarPCB(elPaquete->data);
			destruirPCB(proceso->pcb);
			proceso->pcb = temp;
			//TODO: mutex

			if(proceso->abortado==false){
			pthread_mutex_lock(&mcola_ready);
			queue_push(cola_ready, proceso);
			pthread_mutex_unlock(&mcola_ready);
			sem_post(&sem_ready);
			log_info(logger,"\x1b[3%dmNUCLEO: Recibi proceso %d por fin de quantum, encolando en ready \x1b[0m", ((proceso->pcb->pid)%6+1),proceso->pcb->pid);

			}else{
				log_info(logger,"\x1b[3%dmNUCLEO: Recibi proceso %d por fin de quantum, abortado \x1b[0m", ((proceso->pcb->pid)%6+1),proceso->pcb->pid);

				abortar(proceso);
			}
			//printf("Pasa por aqui");
			queue_push(cola_CPU_libres, (void*)*(int*)socket);
			sem_post(&sem_cpu);
			//printf("Pasa por alla\n");

			break;

		case 320: //
			pthread_mutex_lock(&mcola_exec);
			proceso = dameProceso(cola_exec, *(int*)socket);
			pthread_mutex_unlock(&mcola_exec);
			log_info(logger,"\x1b[3%dmNUCLEO: Recibi proceso %d por fin de ejecucion, encolando en cola exit\x1b[0m", ((proceso->pcb->pid)%6+1),proceso->pcb->pid);
			pthread_mutex_lock(&mcola_exit);
			destruirCONTEXTO(proceso->pcb);
			queue_push(cola_exit, proceso);
			pthread_mutex_unlock(&mcola_exit);
			queue_push(cola_CPU_libres, (void*)*(int*)socket);
			sem_post(&sem_cpu);
			pthread_mutex_lock(&umcm);
			enviar(umc, 6, sizeof(int), &proceso->pcb->pid);
			pthread_mutex_unlock(&umcm);
			enviar(proceso->socket_CONSOLA,162,sizeof(int),&proceso->pcb->pid);
			break;

		case 370: //
			pthread_mutex_lock(&mcola_exec);
			proceso = dameProceso(cola_exec, *(int*)socket);
			pthread_mutex_unlock(&mcola_exec);
			log_info(logger,"\x1b[3%dmNUCLEO: Recibi proceso %d por abortado, encolando en cola exit\x1b[0m", ((proceso->pcb->pid)%6+1),proceso->pcb->pid);
			pthread_mutex_lock(&mcola_exit);
			destruirCONTEXTO(proceso->pcb);
			queue_push(cola_exit, proceso);
			pthread_mutex_unlock(&mcola_exit);
			queue_push(cola_CPU_libres, (void*)*(int*)socket);
			sem_post(&sem_cpu);
			pthread_mutex_lock(&umcm);
			enviar(umc, 6, sizeof(int), &proceso->pcb->pid);
			pthread_mutex_unlock(&umcm);
			enviar(proceso->socket_CONSOLA,164,sizeof(int),&proceso->pcb->pid);
			break;

		case 380:
			pthread_mutex_lock(&mcola_exec);
			proceso = dameProceso(cola_exec, *(int*)socket);
			pthread_mutex_unlock(&mcola_exec);
			int tiempo;
			tiempo=((int*)(elPaquete->data))[0];


			elPaquete2 = recibir(*(int*)socket);
			temp = desserializarPCB(elPaquete2->data);
			destruirPCB(proceso->pcb);
			liberar_paquete(elPaquete2);
			proceso->pcb = temp;
			if(proceso->abortado==false){
			//	((char*)elPaquete->data)[7]='\0';
				//log_info(logger,"COSO %c %c", ((char*)elPaquete->data)[4],((char*)elPaquete->data)[5] );

				log_info(logger,"\x1b[3%dmNUCLEO: Recibi proceso %d mando a bloquear por %s por %d unidades\x1b[0m", ((proceso->pcb->pid)%6+1),proceso->pcb->pid,(char*)(elPaquete->data+4),tiempo);

							pthread_mutex_lock(&mutex_config);
							bloqueoIoManager(proceso, (elPaquete->data)+4, tiempo);
							pthread_mutex_unlock(&mutex_config);

						}else{
							log_info(logger,"\x1b[3%dmNUCLEO: Recibi proceso %d por fin de quantum, abortado \x1b[0m", ((proceso->pcb->pid)%6+1),proceso->pcb->pid);

							abortar(proceso);
						}

			queue_push(cola_CPU_libres, (void*)*(int*)socket);
			sem_post(&sem_cpu);

			break;

		case 341: //Pide semaforo
			pthread_mutex_lock(&mcola_exec);
			proceso = dameProceso(cola_exec, *(int*)socket);
			pthread_mutex_unlock(&mcola_exec);
			pthread_mutex_lock(&mutex_config);
			log_info(logger,"\x1b[3%dmNUCLEO: Proceso %d pide semaforo:%s \x1b[0m", ((proceso->pcb->pid)%6+1),proceso->pcb->pid,elPaquete->data);

			int * valorSemaforo = pideSemaforo(elPaquete->data);
			int mandar;
			if(*valorSemaforo<=0){
				mandar =1;
				log_info(logger,"\x1b[3%dmNUCLEO: Recibi proceso %d mando a bloquear por semaforo %s\x1b[0m", ((proceso->pcb->pid)%6+1),proceso->pcb->pid,elPaquete->data);
				enviar(*(int*)socket, 342, sizeof(int), &mandar);// 1 si se bloquea 0 si no
				elPaquete2 = recibir(*(int*)socket);
				temp = desserializarPCB(elPaquete2->data);
				liberar_paquete(elPaquete2);
				destruirPCB(proceso->pcb);
				proceso->pcb = temp;

				if(proceso->abortado==false){
						bloqueoSemaforo(proceso,elPaquete->data);
						queue_push(cola_CPU_libres, (void*)*(int*)socket);
						sem_post(&sem_cpu);
				}else{
					log_info(logger,"\x1b[3%dmNUCLEO: Recibi proceso %d por fin de quantum, abortado \x1b[0m", ((proceso->pcb->pid)%6+1),proceso->pcb->pid);
						abortar(proceso);
				}

			}else{
				escribeSemaforo(elPaquete->data,*pideSemaforo(elPaquete->data)-1);
				mandar=0;
				enviar(*(int*)socket, 342, sizeof(int), &mandar);//
				pthread_mutex_lock(&mcola_exec);
				queue_push(cola_exec, proceso);
				pthread_mutex_unlock(&mcola_exec);
			}
			

			pthread_mutex_unlock(&mutex_config);
			break;

		case 343: //Libera semaforo
			pthread_mutex_lock(&mcola_exec);
			proceso = dameProceso(cola_exec, *(int*)socket);
			queue_push(cola_exec, proceso);
			pthread_mutex_unlock(&mcola_exec);
			pthread_mutex_lock(&mutex_config);
			log_info(logger,"\x1b[3%dmNUCLEO: Proceso %d libera semaforo:%s \x1b[0m", ((proceso->pcb->pid)%6+1),proceso->pcb->pid,elPaquete->data);

			liberaSemaforo(elPaquete->data);
			pthread_mutex_unlock(&mutex_config);
			break;

		case 350: //Escribe variable
			pthread_mutex_lock(&mcola_exec);
			proceso = dameProceso(cola_exec, *(int*)socket);
			queue_push(cola_exec, proceso);
			pthread_mutex_unlock(&mcola_exec);
			pthread_mutex_lock(&mutex_config);
			escribeVariable(elPaquete->data);
			pthread_mutex_unlock(&mutex_config);
			break;

		case 351: //Pide variable
			pthread_mutex_lock(&mcola_exec);
			proceso = dameProceso(cola_exec, *(int*)socket);
			queue_push(cola_exec, proceso);
			pthread_mutex_unlock(&mcola_exec);
			pthread_mutex_lock(&mutex_config);
			enviar(*(int*)socket, 352, sizeof(int), pideVariable(elPaquete->data));
			pthread_mutex_unlock(&mutex_config);
			break;

		case 360: //imprimir
			pthread_mutex_lock(&mcola_exec);
			proceso = dameProceso(cola_exec, *(int*)socket);
			queue_push(cola_exec, proceso);
			pthread_mutex_unlock(&mcola_exec);
			if(proceso->abortado==false) enviar(proceso->socket_CONSOLA, 160, elPaquete->tamanio, elPaquete->data);
			break;
		case 361: //imprimir texto
			pthread_mutex_lock(&mcola_exec);
			proceso = dameProceso(cola_exec, *(int*)socket);
			queue_push(cola_exec, proceso);
			pthread_mutex_unlock(&mcola_exec);
			if(proceso->abortado==false) enviar(proceso->socket_CONSOLA, 161, elPaquete->tamanio, elPaquete->data);
		}



		liberar_paquete(elPaquete);
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

	//	printf("NUCLEO: Acepte nueva CPU %d\n", *socketClienteTemp);
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
	consola = conectar_a("127.0.0.1", "1210");
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
	cpu = conectar_a("127.0.0.1", "1202");
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




void get_config_nucleo ()
{


	bool tengoIo() {
		bool retorno = false;
		int i;
		for(i=0;i<strlen((char*)config_nucleo->IO_SLEEP)/ sizeof(char*);i++){
			if(list_size(colas_ios[i]->elements)>0) retorno = true;
		}

		return retorno;
	}

	bool tengoSem() {
		bool retorno = false;
		int i;
		for(i=0;i<strlen((char*)config_nucleo->SEM_INIT)/ sizeof(char*);i++){
			if(list_size(colas_semaforos[i]->elements)>0) retorno = true;
		}

		return retorno;
	}



	if(config_nucleo){
		//printf("llegoe\n");
	bool sema; bool io;
	sema = tengoSem();
	io = tengoIo();
	if(sema||io){
		log_info(logger,"NUCLEO: no se puede cambiar la configuracion hasta que las colas de IO y Semaforos se encuentren vacias");
	while(tengoSem()||tengoIo()){}
	log_info(logger,"NUCLEO: Colas vacias, voy a cambiar configuracion de nucleo");
	}

	}else{
		config_nucleo = malloc(sizeof(CONF_NUCLEO));
		//printf("no hay config\n");
	}




	//TODO:Mutex son generales, uso uno solo?
	pthread_mutex_lock(&mutex_config);
	t_config *fnucleo = config_create(CONFIG_NUCLEO);


	//printf("VALOR:%d\n",config_nucleo->PUERTO_PROG);

	/*
	 *
	 *
	 for (i=0; i<10; ++i) {
  free(mat[i]);
		}


	 *
	 */
	config_nucleo->PUERTO_PROG = config_get_string_value(fnucleo, "PUERTO_PROG");

	config_nucleo->PUERTO_CPU = config_get_string_value(fnucleo, "PUERTO_CPU");
	config_nucleo->QUANTUM = config_get_int_value(fnucleo, "QUANTUM");
	config_nucleo->QUANTUM_SLEEP = config_get_int_value(fnucleo, "QUANTUM_SLEEP");
	config_nucleo->IO_IDS = config_get_array_value(fnucleo, "IO_IDS");
	config_nucleo->IO_SLEEP = config_get_array_value(fnucleo, "IO_SLEEP");
	config_nucleo->IO_SLEEP = config_get_array_value(fnucleo, "IO_SLEEP");

	config_nucleo->SEM_IDS = config_get_array_value(fnucleo, "SEM_IDS");
	config_nucleo->SEM_INIT = config_get_array_value(fnucleo, "SEM_INIT");

	//printf("COSOx %d %d %d\n",strlen((char*)config_nucleo->IO_SLEEP),strlen((char*)config_nucleo->IO_IDS),strlen((char*)config_nucleo->SEM_INIT));



	config_nucleo->SHARED_VARS = config_get_array_value(fnucleo, "SHARED_VARS");
	config_nucleo->STACK_SIZE = config_get_int_value(fnucleo, "STACK_SIZE");
	//HASTA ACA LO QUE PIDE EL ENUNCIADO

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
	//printf("TIMERS %d\n",timers);
	if(timers!=0){
		//printf("TIMERS NO NULOS TIMER\n",timers);
		free(timers);
	}
	timers = nalloc(strlen((char*)config_nucleo->IO_SLEEP) * sizeof(char*));
	if(colas_ios!=0){
		//printf("COLAS NO NULOS TIMER\n",timers);
		free(colas_ios);
	}
	colas_ios = nalloc(strlen((char*)config_nucleo->IO_SLEEP) * sizeof(char*));

	int i;
	//printf("COSO %d %d %d\n",strlen((char*)config_nucleo->IO_SLEEP),strlen((char*)config_nucleo->IO_IDS),strlen((char*)config_nucleo->SEM_INIT));
	//sleep(5);
	for (i = 0; i < strlen((char*)config_nucleo->IO_SLEEP)/ sizeof(char*); i++) {

		timers[i] = nalloc(sizeof(timer_t));
		colas_ios[i] = nalloc(sizeof(t_queue*));
		colas_ios[i] = queue_create();
		//printf("crea uno\n");
		//sleep(5);
	}

	if(colas_semaforos!=0){
		//printf("COLAS NO NULOS TIMER\n",timers);
		free(colas_semaforos);
	}
	colas_semaforos = nalloc(strlen((char*)config_nucleo->SEM_INIT) * sizeof(char*));

	for (i = 0; i < strlen((char*)config_nucleo->SEM_INIT)/ sizeof(char*); i++) {

		colas_semaforos[i] = nalloc(sizeof(t_queue*));
		colas_semaforos[i] = queue_create();
	}

	pthread_mutex_unlock(&mutex_config);

	////Ya no lo necesito
	return;
}
int *convertirConfigInt(char **ana1, char **ana2) {
	int i;
	int *resul;
	resul = nalloc(((strlen((char*)ana1)) / sizeof(char*)) * sizeof(int));
	for (i = 0; i < (strlen((char*)ana1)) / sizeof(char*); i++) {
		resul[i] = atoi(ana1[i]);
	}
	return resul;
}
int *convertirConfig0(char **ana1) {
	int i;
	int *resul;
	resul = nalloc(((strlen((char*)ana1)) / sizeof(char*)) * sizeof(int));
	for (i = 0; i < (strlen((char*)ana1)) / sizeof(char*); i++) {
		resul[i] = 0;
	}
	return resul;
}

void * nalloc(int tamanio){
	int i;void * retorno = malloc(tamanio);
	for(i=0;i<tamanio;i++) ((char*)retorno)[i]=0;
	return retorno;
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
