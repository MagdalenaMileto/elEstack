
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

 #include "../../COMUNES/nsockets.c"


void intHandler(int dummy);
int socketCliente,servidorSocket,socketCliente2,servidorSocket2,socketClienteSwap;
char mensaje[100];



void *hilo_NUCLEO(void *arg);
void *hilo_CPU(void *arg);

void main(void){
	 signal(SIGINT, intHandler);
	printf("UMC: INICIÓ\n");
	t_header header;


		pthread_t thNUCLEO, thCPU;

socketClienteSwap= cliente("127.0.0.1",1206);


	pthread_create(&thNUCLEO, NULL, hilo_NUCLEO, NULL);
	pthread_create(&thCPU, NULL, hilo_CPU, NULL);

	//Para evitar que finalice antes que los hilos lo hagan
	pthread_join(thNUCLEO, NULL);
	pthread_join(thCPU, NULL);

}







void *hilo_NUCLEO(void *arg){
//handlear si no hay swap




	servidorSocket=servidor(1200);

	if(servidorSocket==-1){

	printf("UMC: Error en el socket, me cierro\n");
	close(servidorSocket);
	exit(1);

	}

	listen(servidorSocket,5);		// IMPORTANTE: listen() es una syscall BLOQUEANTE.
	struct sockaddr_in addr;			// Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t addrlen = sizeof(addr);

	 socketCliente = accept(servidorSocket, (struct sockaddr *) &addr, &addrlen);


int a=1;
while(a){



	if(leer_socket (socketCliente, mensaje, sizeof(mensaje))>0) {printf("UMC: Recibi NUCLEO %s",mensaje); ;


     //printf("UMC: Cierro\n");
                 // close(socketCliente);
                 // close(servidorSocket);
                
                  
                 //  exit (1);

}


/*
	a=recibir_paquete(socketCliente,&header);
		if(a==-1){
//
                    
              	}else{ 

              		if(header.id == 101){
              			printf("UMC: Recibi %s",(char*)header.data);
              			free(header.data);//Libero esto que me genero recibir paquete
         	  		}
         	  	}


}


*/
}


	//return 0;

}




void *hilo_CPU(void *arg){
//handlear si no hay swap




	servidorSocket2=servidor(1205);

	if(servidorSocket2==-1){

	printf("UMC: Error en el socket, me cierro\n");
	close(servidorSocket2);
	exit(1);

	}

	listen(servidorSocket2,5);		// IMPORTANTE: listen() es una syscall BLOQUEANTE.
	struct sockaddr_in addr;			// Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t addrlen = sizeof(addr);

	 socketCliente2 = accept(servidorSocket2, (struct sockaddr *) &addr, &addrlen);


int a=1;
while(a){



	if(leer_socket (socketCliente2, mensaje, sizeof(mensaje))>0) {printf("UMC: Recibi CPU %s",mensaje); ;


	escribir_socket (socketClienteSwap, mensaje, sizeof(mensaje));


     printf("UMC: Cierro\n");
     sleep(1);
                 close(socketCliente);
                  close(servidorSocket);
                   close(socketCliente2);
                  close(servidorSocket2);
                   close(socketClienteSwap);
              
                
                  
                   exit (1);

}


/*
	a=recibir_paquete(socketCliente,&header);
		if(a==-1){
//
                    
              	}else{ 

              		if(header.id == 101){
              			printf("UMC: Recibi %s",(char*)header.data);
              			free(header.data);//Libero esto que me genero recibir paquete
         	  		}
         	  	}


}


*/
}


	//return 0;

}






void intHandler(int dummy) {
	//clrscr();

	close(servidorSocket);
	close(socketCliente);
  printf("cierro Todo...\n\n");
  exit(0);
}


