/*
 * nucleo.c
 *
 *  Created on: 23/4/2016
 *      Author: nico
 */

#include "nucleo.h"


int listenningSocket,socketCliente,servidorSocket,servidorCPU,clienteSocket,losClientes,clientesCPU,umc,ultimoCPU;

int main(){



	   signal(SIGINT, intHandler);
	pthread_t thCONSOLA, thCPU;








	umc = cliente("127.0.0.1",1200);
	if(umc==0){
		printf("No encontre UMC me cierro :'( %d\n",(int)&umc);
	  exit (EXIT_FAILURE);
	}







	pthread_create(&thCONSOLA, NULL, hilo_CONSOLA, NULL);
	pthread_create(&thCPU, NULL, hilo_CPU, NULL);

	//Para evitar que finalice antes que los hilos lo hagan
	pthread_join(thCONSOLA, NULL);
	pthread_join(thCPU, NULL);


	return 0;
	
}


void *hilo_CPU(void *arg){

	fd_set active_fd_set, read_fd_set; size_t size;
	int i;int32_t retorno;
	t_header header;


	//Timeout del select
	struct timeval tv;			// Estructura para select()
	tv.tv_sec = 2;
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
              			printf("Recibi %s",(char*)header.data);
                    enviar_paquete(umc, header);
                    enviar_paquete(ultimoCPU, header);

              			free(header.data);//Libero esto que me genero recibir paquete
         	  		}

         		}

              }
          }
    }





}

void *hilo_CONSOLA(void *arg){

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

  printf("cierro Todo...\n\n");
  exit(0);
}

