
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>

 #include "../../COMUNES/nsockets.c"


void intHandler(int dummy);
int socketCliente,servidorSocket;
char mensaje[100];


void main(void){
	 signal(SIGINT, intHandler);
	printf("UMC: INICIÓ\n");
	t_header header;
	printf("UMC: Me voy a bloquear hasta que conecte nucleo\n");


	servidorSocket=servidor(1200);


	listen(servidorSocket,5);		// IMPORTANTE: listen() es una syscall BLOQUEANTE.
	struct sockaddr_in addr;			// Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t addrlen = sizeof(addr);

	 socketCliente = accept(servidorSocket, (struct sockaddr *) &addr, &addrlen);


int a;
while(1){



	if(leer_socket (socketCliente, mensaje, sizeof(mensaje))>0) printf("UMC: Recibi %s",mensaje); ;


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


