/*
 * consola.c
 *
 *  Created on: 23/4/2016
 *      Author: utnso
 */

#include "funcionesConsola.h"

int PUERTO_NUCLEO = 9997;
int socketConexionNucleo;


// estaria muy bueno hacer el log..


int main(int argc, char **argv) {
	
	if (argc != 2)
		{
			perror("No se paso la cantidad de parametros necesaria\n");
			return EXIT_FAILURE;
		}


	char* script;
	FILE *archivo;
	char nomArchivo[50];
	signed int nucleo;
	signed int estado;
	signed int consola;


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
	fread(script, fsize, 1, archivo);
	script[fsize] = '\0';
	return script;

}

int conectarConElNucleo(){


int nucleo = cliente("127.0.0.1", PUERTO_NUCLEO);

if(nucleo==0){
	printf("CONSOLA: No encontre NUCLEO\n");
	exit (EXIT_FAILURE);
	}

printf("CONSOLA: Conecta bien nucleo %d\n", nucleo);


        int estado;
        estado=handshake(nucleo, 102,101);

        if(estado==1){
            printf("CONSOLA:Handshake exitoso con nucleo\n");
        }else{
            printf("CONSOLA:Handshake invalido con nucleo\n");
        }

return nucleo;
}

int enviarInformacionAlNucleo(char * script, signed int nucleo, signed int consola){

	t_header header,headerEnviar;


	 while(1){

	     	if(header.id==103){

	     		headerEnviar.id=205;int a=1;
	     		headerEnviar.data=&a;
	     		headerEnviar.size=sizeof(int);
	     		enviar_paquete(nucleo,headerEnviar);

	     	}

	     	 if(header.id==108){

	     	recibir_paquete(nucleo,&header);

	     	}

	     //	printf("Pase\n");
	     	free(header.data);

	 	 }

return 0;
}







