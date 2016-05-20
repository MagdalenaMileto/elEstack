/*
 * consola.c
 *
 *  Created on: 23/4/2016
 *      Author: utnso
 */

#include "funcionesConsola.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <commons/config.h>
#include "../../COMUNES/handshake.h"
#include "../../COMUNES/nsockets.h"



//int main(void) {

	//struct sockaddr_in direccionServidor;
	//direccionServidor.sin_family = AF_INET;
	//direccionServidor.sin_addr.s_addr = inet_addr("127.0.0.1");
	//direccionServidor.sin_port = htons(1201);

	//int consola = socket(AF_INET, SOCK_STREAM, 0);

	//if (connect(consola, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		//	perror("CONSOLA: No se pudo conectar/ cerrando...");
		//close(consola);
		//return 1;
	//}

	//char mensaje[100]="Entrega 30/4 \n";
	//send(consola, mensaje, sizeof(char)*100, 0);
	//printf("CONSOLA: envio %s",mensaje);

	//close(consola);
	//printf("CONSOLA: Cierro\n");
	//return 0;
//}


int main(int argc, char **argv){

	char* script;
	FILE *archivo;
	char nomArchivo[];
	int exito = 0;
	char comando [100];


	int consola = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in direccionServidor;
			direccionServidor.sin_family = AF_INET;
			direccionServidor.sin_addr.s_addr = inet_addr("127.0.0.1");
			direccionServidor.sin_port = htons(1201);

	if (connect(consola, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
			perror("CONSOLA: No se pudo conectar/ cerrando...");
		close(consola);
		break;
		return 1;
	}

	typedef struct{
		int32_t id;
		int32_t size;
		void* data;
	}t_header;


 			//el programa inicio
			//abriendo el script...

		strcpy(nomArchivo,argv[1]);

		if ((archivo = fopen("nomArchivo", "r")) == NULL){
			printf("No se pudo acceder al script.\n");
		}
		else{
		script = leerarchivo(archivo);					//lee el archivo y reserva memoria
				fclose(archivo);
				printf("Exito en lectura de scrit.\n");
				exito=1;
		}



	if(exito){

		if(conexionConElNucleo != 0){

			if(handshake(consola,102,101) != 1){
				printf("CONSOLA; handshake invalido nucleo %d\n", consola);
				return EXIT_FAILURE;
			}
			printf("CONSOLA: handshake valido nucleo %d\n", consola);

			t_header header;

			header.size=sizeof(script);
			header.id = 103;
			header.data = script;

			enviar_paquete(consola,header);

			recibir_paquete(consola,&header);


		}
	}

	free(script);
	close(consola);
	printf("CONSOLA: Cierro\n");
	return 0;
}


//************************************FUNCONES*********************************************


struct sockaddr_in
	{
	   short int sin_family;        // Familia de la Dirección
	   unsigned short int sin_port; // Puerto
	   struct in_addr sin_addr;     // Dirección de Internet
	}direccionServidor;






int conexionConElNucleo(void){

int nucleo, n,conexion;
if(leerArchivoDeConfiguracion()!=0){

		n= leerArchivoDeConfiguracion();
		nucleo = cliente(n,9997);

		if(nucleo==0){
			printf("CONSOLA: No hubo conexion con el nucleo.. :'( \n");
			conexion = 0;
			return conexion;
		}
		else {
			printf("CONSOLA: Conexion con nucleo %d\n",nucleo);
			conexion = 1;
			return conexion;
		}
}
else return 0;
}




char* leerArchivo(FILE *archivo) {

	fseek(archivo, 0, SEEK_END);
	long tamanio = ftell(archivo);
	fseek(archivo, 0, SEEK_SET);   // reserva memoria
	char *script = malloc(tamanio + 1);
	size_t leer = fread(script, tamanio, 1, archivo);
	script[tamanio] = '\0';
	printf("Lectura: %s\n", script);
	return script;

}


