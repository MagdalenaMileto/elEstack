
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>


 #include "../../COMUNES/nsockets.c"

int servidorSocket;

void main(void){

	t_header header;
servidorSocket=servidor(1200); 


	listen(servidorSocket,5);		// IMPORTANTE: listen() es una syscall BLOQUEANTE.
	struct sockaddr_in addr;			// Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t addrlen = sizeof(addr);

	int socketCliente = accept(servidorSocket, (struct sockaddr *) &addr, &addrlen);
	puts("cliente");
int a;
while(1){


	a=recibir_paquete(socketCliente,&header);
		if(a==-1){
//
                    
              	}else{ 

              		if(header.id == 101){
              			printf("Recibi %s",header.data);
              			free(header.data);//Libero esto que me genero recibir paquete
         	  		}
         	  	}


}





	//return 0;

}