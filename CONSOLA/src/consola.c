/*
 * consola.c
 *
 *  Created on: 23/4/2016
 *      Author: utnso
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<sys/types.h>
#include <netinet/in.h>
#include<netinet/ip.h>
#include<unistd.h>

int main(void) {


	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr("127.0.0.1");
	direccionServidor.sin_port = htons(1201);

	int consola = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(consola, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("CONSOLA: No se pudo conectar/ cerrando...");
		close(consola);
		return 1;
	}


	char mensaje[100]="Entrega 30/4 \n";

	//printf("****%d**",sizeof(mensaje));

	
	send(consola, mensaje, sizeof(char)*100, 0);
		
	printf("CONSOLA: envio %s",mensaje);

	close(consola);
	printf("CONSOLA: Cierro\n");
	return 0;
}



int lecturaDeScript(int argc, char **argv)
{
	FILE* script;
	char buffer[100];

  if(argc==1)
  {
    printf("Debe ingresar el nombre del archivo fuente (en lenguaje AnSISOP) en linea de comando.\n");
    exit(1);
  }

  if(argc!=3)
  {
	  printf("Numero incorrecto de argumentos.\n");
  }

  if((script= fopen(argv[1],"r"))==NULL)
  {
    printf("Error al abrir %s\n",argv[1]);
    exit(2);
  }

  printf("Cantidad de bytes en el archivo %s: %ld\n",argv[1],sizeof(argv[1]));

  if(sizeof(argv[1]) > 0)
  {
	  script = fopen (argv[1], "r");

	   	while (feof(script) == 0)
	   	{
	   		fscanf(script, "%s" ,buffer);
	   		printf("%s",buffer);
	   	}

	   	fclose (script);
  }
  return 0;
}


