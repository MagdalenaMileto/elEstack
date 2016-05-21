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
	char nomArchivo[50];
	int exito = 0;

	int conexionConElNucleo(void);
	char* leerArchivo(FILE *archivo);
	void leerArchivoDeConfiguracion();


	int consola = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in direccionServidor;
			direccionServidor.sin_family = AF_INET;
			direccionServidor.sin_addr.s_addr = inet_addr("127.0.0.1");
			direccionServidor.sin_port = htons(1201);

	if (connect(consola, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
			perror("CONSOLA: No se pudo conectar/ cerrando...");
		close(consola);
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
				script = leerArchivo(archivo);					//lee el archivo y reserva memoria
				fclose(archivo);
				printf("Exito en lectura de scrit.\n");
				exito=1;
		}



	if(exito){


			if(handshake(consola,102,101) != 1){
				printf("CONSOLA; handshake invalido nucleo %d\n", consola);
				return EXIT_FAILURE;
			}
			printf("CONSOLA: handshake valido nucleo %d\n", consola);

			t_header header;

			header.size=sizeof(script);
			header.id = 103;
			header.data = script;


			enviar_paquete(consola, header);

			if ( recibir_paquete(consola,&header) < 0)return EXIT_FAILURE;

			printf("%s\n", header.data);



}
	free(script);
	close(consola);
	printf("CONSOLA: Cierro\n");
	return 0;
}


//************************************FUNCONES*********************************************




int conexionConElNucleo(void){

int nucleo;
t_config* configuracion;

configuracion = config_create("Ruta al archivo de configuracion");
int PUERTO_NUCLEO;

		if (config_has_property(configuracion,"PUERTO_NUCLEO")){
			PUERTO_NUCLEO = config_get_int_value(configuracion,PUERTO_NUCLEO);

				nucleo = cliente(PUERTO_NUCLEO,9997);

				if(nucleo==0){
			printf("CONSOLA: No hubo conexion con el nucleo.. :'( \n");
			return 0;


		}
		else {
			printf("CONSOLA: Conexion con nucleo %d\n",nucleo);
			return 1;
		}
	}
return 1;
}








char* leerArchivo(FILE *archivo) {

	fseek(archivo, 0, SEEK_END);
	long tamanio = ftell(archivo);
	fseek(archivo, 0, SEEK_SET);   // reserva memoria
	char *script = malloc(tamanio + 1);
	script[tamanio] = '\0';
	printf("Lectura: %s\n", script);
	return script;

}


