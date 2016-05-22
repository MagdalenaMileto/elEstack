/*
 * consola.c
 *
 *  Created on: 23/4/2016
 *      Author: utnso
 */
#include "funcionesConsola.h"
char *direccion;
int puerto;
int socketConexionNucleo;

// estaria muy bueno hacer el log..


int main(int argc, char **argv) {

	char* script;
	FILE *archivo;
	char nomArchivo[50];
	int32_t nucleo;
	int32_t estado;
	int32_t consola;


	consola=iniciarConsola();

	if(consola!=1){

		strcpy(nomArchivo,argv[1]);

				if ((archivo = fopen("nomArchivo", "r")) == NULL){
					printf("No se pudo acceder al script.\n");
					return EXIT_FAILURE;
				}

				else{

						script = leerElArchivo(archivo);					//lee el archivo y reserva memoria
						fclose(archivo);
						printf("Exito en lectura de scrit.\n");


				}

	nucleo= conectarConElNucleo();
	estado= enviarInformacionAlNucleo(script, nucleo, consola);

	if(estado>0) printf("se envio script al nucleo.\n");


free(script);
close(consola);
printf("CONSOLA: Cierro\n");
	}


return 0;
}



//********************************************FUNCIONES********************************************


int iniciarConsola() {

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
	return consola;
}




char* leerElArchivo(FILE *archivo) {

	fseek(archivo, 0, SEEK_END);
	long fsize = ftell(archivo);
	fseek(archivo, 0, SEEK_SET);
	char *script = malloc(fsize + 1);
	size_t resp = fread(script, fsize, 1, archivo);
	script[fsize] = '\0';
	return script;

}

int conectarConElNucleo(){


int nucleo = cliente(direccion, puerto);

		//	if(handshakeOut('c','n',nucleo))
		//	{
		//		perror("No me conecte con un nucleo\n");
		//		close(nucleo);
		//		return 1;
		//	}
return nucleo;
}



int enviarInformacionAlNucleo(char * script, int32_t nucleo, int32_t consola){

	typedef struct{
			int32_t id;
			int32_t size;
			void* data;
		}t_header;


	 t_header header;
	header.id = 102;
	header.size = sizeof(script);
	header.data = &script;

	int32_t respuesta=0;

	int32_t estado=0;

	printf("***%d*\n",*((int*)header.data));

	estado = enviar_paquete(nucleo, header);

	while(1){


	respuesta=recibir_paquete(consola,&header);   // se quedara esperando resuesta?? eso esta bien?

		if (respuesta < 0)return EXIT_FAILURE;
				else printf("%s\n", header.data);

			free(header.size);
		}

return estado;
}


