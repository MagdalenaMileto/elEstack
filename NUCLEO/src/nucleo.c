/*
 * nucleo.c
 *
 *  Created on: 23/4/2016
 *      Author: nico
 */

#include "nucleo.h"


/* VARIABLES GLOBALES */


struct arg_struct {
    int socket;
   
};

typedef struct{
  int socket_CPU;
  int socket_PROGRAMA;
  t_pcb pcb;
}t_proceso;





int listenningSocket,socketCliente,servidorSocket,servidorCPU,clienteSocket,losClientes,clientesCPU,umc,ultimoCPU;

char mensaje[100];


/* FIN DE VARIABLES GLOBALES */





int main(){

	printf("NUCLEO: INICIÓ\n");

  signal(SIGINT, intHandler);


  //conectarUmc();



	pthread_t thPCP, thPLP,thCONEXIONES_CPU,thCONEXIONES_CONSOLA;

	pthread_create(&thCONEXIONES_CONSOLA, NULL, hilo_CONEXIONES_CONSOLA, NULL);
 // pthread_create(&thCONEXIONES_CPU, NULL, hilo_CONEXIONES_CPU, NULL);
  //pthread_create(&thPCP, NULL, hilo_PCP, NULL);
  //pthread_create(&thPLP, NULL, hilo_PLP, NULL);

 pthread_join(thCONEXIONES_CONSOLA, NULL);
 // pthread_join(thCONEXIONES_CPU, NULL);
	//pthread_join(thPCP, NULL);
	//pthread_join(thPLP, NULL);

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

void *hilo_CONEXION_CONSOLA(void *arg){

struct arg_struct *args = (struct arg_struct *)arg;


  while(1){
    printf("Hilo del socket%d\n",args->socket);sleep(1);
  }
}

void *hilo_CONEXIONES_CONSOLA(void *arg){



int servidorSocket,socketCliente;
struct sockaddr_in addr;      // Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
socklen_t addrlen = sizeof(addr);


  servidorSocket=servidor(1200);

  if(servidorSocket==-1){

  printf("UMC: Error en el socket, me cierro\n");
  close(servidorSocket);
  exit(1);

  }





  listen(servidorSocket,5);   // IMPORTANTE: listen() es una syscall BLOQUEANTE.


while(1){

   socketCliente = accept(servidorSocket, (struct sockaddr *) &addr, &addrlen);
 


 struct arg_struct *args;

args = malloc(sizeof(struct arg_struct));




   args->socket=socketCliente;



pthread_t thCONEXION_CONSOLA;

  pthread_create(&thCONEXION_CONSOLA, NULL, hilo_CONEXION_CONSOLA, (void *)args);


printf("acepte%d\n",socketCliente);
}






t_proceso* proceso_aux;
proceso_aux = malloc(sizeof(t_proceso));








}



void caca (void){

  fd_set active_fd_set, read_fd_set; size_t size;
  int i;int32_t retorno;
//  t_header header;


  //Timeout del select
  struct timeval tv;      // Estructura para select()
  tv.tv_sec = 5;
  tv.tv_usec = 500000;

  

  servidorSocket=servidor(1201);  //Creo servidor escucha en puerto 1233

  //Config del select

  FD_ZERO (&active_fd_set);
  FD_SET (servidorSocket, &active_fd_set);



  
  while (1)
    {
      /* Block until input arrives on one or more active sockets. */
      read_fd_set = active_fd_set;
      if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, &tv) < 0)
        {
        printf("Error: probablemente el puerto esta ocupado\n");
          exit (EXIT_FAILURE);
        }

      /* Service all the sockets with input pending. */
      for (i = 0; i < FD_SETSIZE; ++i)
        if (FD_ISSET (i, &read_fd_set))
          {
            if (i == servidorSocket)
              {
                /* Connection request on original socket. */
                int new;
                size = sizeof (losClientes);
                new = accept (servidorSocket,
                              (struct sockaddr *) &losClientes,
                              &size);
                if (new < 0)
                  {
                    printf("Problema con el select 2\n");
                    exit (EXIT_FAILURE);
                  }
              
                FD_SET (new, &active_fd_set);
              }
            else
              { //hay evento

                //retorno= recibir_paquete(i, &header);
                retorno = leer_socket (i, mensaje, sizeof(mensaje));
                if(retorno==-1){
                  //El evento fue que el socket cliente se cerro
                  close( i );
                  FD_CLR (i, &active_fd_set);
                    
                }else{ 
                  printf("NUCLEO: Recibi %s",mensaje);FD_CLR(i,&active_fd_set);

                  escribir_socket (umc, mensaje, sizeof(mensaje));
                  escribir_socket (ultimoCPU, mensaje, sizeof(mensaje));
                  printf("NUCLEO: Cierro\n");
                  close(servidorSocket);
                  close(servidorCPU);
                  close(umc); 
                  
                   exit (1);

                  /*
                  if(header.id == 101){
                    printf("Recibi %s",(char*)header.data);
                    enviar_paquete(umc, header);
                    enviar_paquete(ultimoCPU, header);

                    free(header.data);//Libero esto que me genero recibir paquete
                }
                */

            }

              }
          }
    }


}

void *hilo_CONEXIONES_CPU(void *arg){

  fd_set active_fd_set, read_fd_set; size_t size;
  int i;int32_t retorno;
  t_header header;


  //Timeout del select
  struct timeval tv;      // Estructura para select()
  tv.tv_sec = 2;
  tv.tv_usec = 500000;

  

  servidorCPU=servidor(1202);  //Creo servidor escucha en puerto 1233

  //Config del select

  FD_ZERO (&active_fd_set);
  FD_SET (servidorCPU, &active_fd_set);



  
  while (1)
    {
      /* Block until input arrives on one or more active sockets. */
      read_fd_set = active_fd_set;
      if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, &tv) < 0)
        {
          exit (EXIT_FAILURE);
        }

      /* Service all the sockets with input pending. */
      for (i = 0; i < FD_SETSIZE; ++i)
        if (FD_ISSET (i, &read_fd_set))
          {
            if (i == servidorCPU)
              {
                /* Connection request on original socket. */
                int new;
                size = sizeof (clientesCPU);
                new = accept (servidorCPU,
                              (struct sockaddr *) &clientesCPU,
                              &size);
                ultimoCPU=new;
                if (new < 0)
                  {
                    exit (EXIT_FAILURE);
                  }
              
                FD_SET (new, &active_fd_set);
              }
            else
              { //hay evento

                retorno= recibir_paquete(i, &header);
                if(retorno==-1){
                  //El evento fue que el socket cliente se cerro
                  close( i );
                  FD_CLR (i, &active_fd_set);
                    
                }else{ 
                  if(header.id == 101){
                  //recibo paquete cpus
                }

            }

              }
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

