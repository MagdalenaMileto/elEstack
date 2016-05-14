/*
 * nucleo.c
 *
 *  Created on: 23/4/2016
 *      Author: nico
 */

#include "nucleo.h"


/* VARIABLES GLOBALES */


//Colas PLP
t_queue* cola_new;
t_queue* cola_exit;

//Colas PCP
t_queue* cola_exec;
t_queue* cola_ready;
t_queue* cola_block;

pthread_mutex_t mutex_cola_new;




// Listas que maneja el PCP
t_queue* cola_CPU_libres;


 struct arg_struct {
  int socket;

};

typedef struct{
  int socket_CPU;
  int socket_PROGRAMA;
  t_pcb pcb;
}t_proceso;




//Eliminar todo lo que sea de mas de aca
int listenningSocket,socketCliente,servidorSocket,servidorCPU,clienteSocket,losClientes,clientesCPU,umc,ultimoCPU;

char mensaje[100];


/* FIN DE VARIABLES GLOBALES */





int main(){
      signal(SIGINT, intHandler);

      printf("NUCLEO: INICIÓ\n");

     
      //Inicializaciones -> lo podriamos meter en una funcion externa

      cola_new =queue_create();
      cola_exec =queue_create();
      cola_ready = queue_create();
      cola_block = queue_create();
      cola_exit = queue_create();
      cola_CPU_libres = queue_create();
 
      pthread_mutex_init(&mutex_cola_new,NULL);

      //conectarUmc();

      pthread_t thPCP, thPLP,thCONEXIONES_CPU,thCONEXIONES_CONSOLA;

      pthread_create(&thCONEXIONES_CONSOLA, NULL, hilo_HANDLER_CONEXIONES_CONSOLA, NULL);
      pthread_create(&thCONEXIONES_CPU, NULL, hilo_HANDLER_CONEXIONES_CPU, NULL);
  

      pthread_create(&thPCP, NULL, hilo_PCP, NULL);
      pthread_create(&thPLP, NULL, hilo_PLP, NULL);

      pthread_join(thCONEXIONES_CONSOLA, NULL);
      pthread_join(thCONEXIONES_CPU, NULL);

      pthread_join(thPCP, NULL);
	pthread_join(thPLP, NULL);

	//Terminamos
}



void conectarUmc(void){

  //Handlear errores; El programa debe salir si no conecta umc? remite conexion? que hacemo?

  umc = cliente("127.0.0.1",1200);
  if(umc==0){
    printf("NUCLEO: No encontre UMC me cierro :'( \n");
      exit (EXIT_FAILURE);
 }

}




void *hilo_PLP(void *arg){

    while(1){
    //Do something yoooo
    }

}



void *hilo_PCP(void *arg){

    while(1){
    //Do something yoooo
    }

}


t_proceso* crearPrograma(void){

      t_proceso* procesoNuevo;
      procesoNuevo=malloc(sizeof(t_proceso));
      return procesoNuevo;

}



/********************************************************************************
*********************************************************************************
*********************************************************************************
************************CONSOLA STUFF********************************************
*********************************************************************************
*********************************************************************************
*********************************************************************************
*/

void *hilo_CONEXION_CONSOLA(void *arg){

    struct arg_struct *args = (struct arg_struct *)arg;
    int estado;
    t_proceso* proceso;

    proceso = crearPrograma();

      while(1){
            t_header estructuraARecibir;
            estado=recibir_paquete(args->socket,&estructuraARecibir);

            if(estado==-1){
                  printf("Nucleo: Cerro Socket consola\n");
                  break;
                  //Aca deberia eliminar el programa pcb cerrar socket blablabla si ejecuta cpu decile ya fue man UMC
            }

      
            if(estructuraARecibir.id==101){
            
                  pthread_mutex_lock(&mutex_cola_new);
                  queue_push(cola_new, proceso);
                  pthread_mutex_unlock(&mutex_cola_new);

                  printf("NUCLEO: Recibi CONSOLA %s\n",(char*)estructuraARecibir.data);
            }

          
            estructuraARecibir.id=0;
            free(estructuraARecibir.data);
          
     }

}



void *hilo_HANDLER_CONEXIONES_CONSOLA(void *arg){

      int servidorSocket,socketCliente;
      struct sockaddr_in addr;      // Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
      socklen_t addrlen = sizeof(addr);
      struct arg_struct *args; //Probar de sacar afuera esto?

      servidorSocket=servidor(1200);


     if(servidorSocket==-1){
          printf("NUCLEO: No puedo crear socket escucha 1200, me cierro\n");//cambiar a p error
          close(servidorSocket);
          exit(1);
     }
     
     listen(servidorSocket,5);    //Aca maximas conexiones, ver de cambiar?

     while(1){
          socketCliente = accept(servidorSocket, (struct sockaddr *) &addr, &addrlen);
          
          args = malloc(sizeof(struct arg_struct));//cuando se termine el proceso hacer un free de esto
          args->socket=socketCliente;

          pthread_t thCONEXION_CONSOLA;
          pthread_create(&thCONEXION_CONSOLA, NULL, hilo_CONEXION_CONSOLA, (void *)args);

          printf("acepte%d\n",socketCliente);
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

void *hilo_CONEXION_CPU(void *arg){

    struct arg_struct *args = (struct arg_struct *)arg;
    int estado;
    t_proceso* proceso;

  while(1){

          t_header estructuraARecibir;
          estado=recibir_paquete(args->socket,&estructuraARecibir);

          if(estado==-1){
               printf("Nucleo: Cerro Socket cpu\n");
               break;
               //Aca deberia eliminar el programa pcb cerrar socket blablabla si ejecuta cpu decile ya fue man UMC
          }

          
          if(estructuraARecibir.id==101){

          }
          estructuraARecibir.id=0;
          free(estructuraARecibir.data);
          
     }

}




void *hilo_HANDLER_CONEXIONES_CPU(void *arg){

     struct arg_struct *args; //Probar de sacar afuera esto?
     int servidorSocket,socketCliente;
     struct sockaddr_in addr;      // Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
     socklen_t addrlen = sizeof(addr);


     servidorSocket=servidor(1201);


     if(servidorSocket==-1){
          printf("NUCLEO: No puedo crear socket escucha 1201, me cierro\n");//cambiar a p error
          close(servidorSocket);
          exit(1);
     }
     
     listen(servidorSocket,5);  //Aca maximas conexiones, ver de cambiar?

     while(1){
          socketCliente = accept(servidorSocket, (struct sockaddr *) &addr, &addrlen);

          
          args = malloc(sizeof(struct arg_struct));//cuando se termine el proceso hacer un free de esto
          args->socket=socketCliente;

          pthread_t thCONEXION_CPU;
          pthread_create(&thCONEXION_CPU, NULL, hilo_CONEXION_CPU, (void *)args);

          printf("acepte%d\n",socketCliente);
     }

}




}
void intHandler(int dummy) {
	//clrscr();

	//close(clienteSocket);
    close(servidorSocket);
    close(servidorCPU);
    close(umc);

    printf("NUCLEO: CERRÓ\n");
    printf("cierro Todo...\n\n");
    exit(0);
}

