
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>


#include "../../COMUNES/nsockets.c"

#define IP "127.0.0.1"
#define PUERTO "9997"
#define PACKAGESIZE 1024	// Define cual va a ser el size maximo del paquete a enviar

#include <signal.h>

int serverSocket;
void intHandler(int dummy);
int main(){
	printf("CPU: INICIÓ\n");
//	sleep(2);
 signal(SIGINT, intHandler);
int a;

serverSocket = cliente("127.0.0.1",1202);

	if(serverSocket==0){
		printf("CPU No encontre NUCLEO me cierro :'( \n",&serverSocket);
	  exit (EXIT_FAILURE);
	}
	

	int enviar = 1;
	char message[PACKAGESIZE];
	t_header header;

//	printf("Conectado al servidor. Bienvenido al sistema, ya puede enviar mensajes. Escriba 'exit' para salir\n");

	while(1){


	a=recibir_paquete(serverSocket,&header);
		if(a==-1){
//
                    
              	}else{ 

              		if(header.id == 101){
              			printf("CPU: Recibi %s",(char*)header.data);
              			free(header.data);//Libero esto que me genero recibir paquete
         	  		}
         	  	}


}







	close(serverSocket);
	return 0;

	
}



void intHandler(int dummy) {
	//clrscr();
	
	close(serverSocket);


  printf("cierro Todo...\n\n");
  exit(0);
}


