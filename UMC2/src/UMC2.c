/*
 ============================================================================
 Name        : UMC2.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>



#include "../../COMUNES/nsockets.c"


#include <signal.h>


void *hilo_Conexion(void *arg);

void *hilo_mock(void *arg);



struct arg_struct {
	int socket;
};




int main(void) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */


    pthread_t thMOCK;
	          pthread_create(&thMOCK, NULL, hilo_mock, NULL);



	 int servidorSocket,socketCliente;
	     struct sockaddr_in addr;
	     socklen_t addrlen = sizeof(addr);
	     struct arg_struct *args;


	     servidorSocket=servidor(5002);


	     if(servidorSocket==-1){
	          printf("El AMEO NO ME DEJA CREAR SOCKET RE GATO\n");
	          close(servidorSocket);
	          exit(1);
	     }

	     listen(servidorSocket,5);

	     while(1){


	          socketCliente = accept(servidorSocket, (struct sockaddr *) &addr, &addrlen);


	          args = malloc(sizeof(struct arg_struct));//cuando se termine el proceso hacer un free de esto
	          args->socket=socketCliente;

	          pthread_t thCONEXION;
	          pthread_create(&thCONEXION, NULL, hilo_Conexion, (void *)args);

	          printf("Acepté cliente: %d\n",socketCliente);
	     }







}






void *hilo_Conexion(void *arg){

    struct arg_struct *args = (struct arg_struct *)arg;
    int estado;


    while(1){

        t_header estructuraARecibir;
        sleep(1);

          estado=recibir_paquete(args->socket,&estructuraARecibir);

          	  //Podes hacer un switch aca

          if(estado==-1){  //Evento desconexion

               printf("Cerró Conexion %d\n",args->socket);
               //Deberia hacer todos los free del hilo
               break;

          }


          if(estado==101){									//Puedo enviar varios paquetes, PCB, handshake, etc. con esto identifico cual es
            	printf("Recibi %s\n",(char*)estructuraARecibir.data);

          }






          //Lo que recibi lo elimino
          estructuraARecibir.id=0; //Por las dudas, creo que no haria falta
          free(estructuraARecibir.data);

     }

     return 0;

}








char mensaje[100]="HOLA QUE TAL\n";

void *hilo_mock(void *arg){

		sleep(1);
	int cliente2;
	cliente2 = cliente("127.0.0.1",5002);

	if(cliente2==0){
		printf("No encontre conexion me cierro :'( \n");
	  	exit (EXIT_FAILURE);
	}



	t_header header;

	header.id = 101;
	header.size = strlen(mensaje);
	header.data = mensaje;


	sleep(1);

	printf("MOCK:Envie %s\n",mensaje);
	enviar_paquete(cliente2, header);
	close(cliente2);

}

