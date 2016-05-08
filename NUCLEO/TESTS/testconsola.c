
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
	printf("CONSOLA: INICIÓ\n");
//	sleep(2);
 signal(SIGINT, intHandler);

//char valorAEnviar [30]="Entrega 30/4 \n\0";

char mensaje[100]="Entrega 30/4 \n";


serverSocket = cliente("127.0.0.1",1200);

	if(serverSocket==0){
		printf("CONSOLA: No encontre NUCLEO me cierro :'( \n");
	  exit (EXIT_FAILURE);
	}
	

	int enviar = 1;
	char message[PACKAGESIZE];
	t_header header;


		/* No usamos header
		header.id = 101;
		header.size = strlen(valorAEnviar) + 1;
		header.data = valorAEnviar;
		*/
		printf("CONSOLA: envie %s",mensaje);
		
	//	escribir_socket (serverSocket, mensaje, sizeof(mensaje));

		//enviar_paquete(serverSocket, header);



while(1){}



	close(serverSocket);
	return 0;

	
}



void intHandler(int dummy) {
	//clrscr();
	
	close(serverSocket);
  printf("cierro Todo...\n\n");
  exit(0);
}


