/*
 * consola.c
 *
 *  Created on: 23/4/2016
 *      Author: utnso
 */


#include "funcionesConsola.h"

int PUERTO_NUCLEO = 9997;
int socketConexionNucleo;
int programa_finalizado=0;

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
	archivoDeConfiguracion();

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
	}

free(script);
t_paquete *paquete=malloc(sizeof(t_paquete));
char* info_cadena;
int info_variable;
int codigo_op;

while(!programa_finalizado){
		paquete = recibir(nucleo);
		memcpy(codigo_op, paquete->codigo_operacion, 4);


		switch(codigo_op) {
		case 101:
				 memcpy(info_cadena, paquete->data, sizeof(paquete->data));
				 puts(info_cadena);
				 break;
		case 102:
				memcpy(info_variable, paquete->data, 4);
				printf("%d\n", info_variable);
				break;
		case 103:
				puts("Programa Finalizado");
				programa_finalizado=1;
				break;
		}

}

free(paquete);
close(consola);
printf("CONSOLA: Cierro\n");



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

void archivoDeConfiguracion() {

	t_config * archivo_configuracion = config_create("./consola.confg");

	puerto_consola = config_get_string_value(archivo_configuracion, "PUERTO");
	ip_nucleo = config_get_string_value(archivo_configuracion, "IP_NUCLEO");
	puerto_nucleo = config_get_string_value(archivo_configuracion, "PUERTO_NUCLEO");

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

	int nucleo= conectar_a(ip_nucleo, puerto_nucleo);

	if(nucleo==0){
		printf("CONSOLA: No encontre NUCLEO\n");
		exit (EXIT_FAILURE);
	}

	printf("CONSOLA: Conecta bien nucleo %d\n", nucleo);


        if(realizar_handshake(nucleo)){
            printf("CONSOLA:Handshake exitoso con nucleo\n");
        }else{
            printf("CONSOLA:Handshake invalido con nucleo\n");
        }

return nucleo;
}


int enviarInformacionAlNucleo(char * script, signed int nucleo, signed int consola){

	t_paquete* paquete,paqueteEnviar;


	 while(1){

	     	if(paquete->codigo_operacion==103){   //codigo operacion no oficial

	     		paqueteEnviar.codigo_operacion=205;  //codigo operacion no oficial
	     		paqueteEnviar.data=&script;
	     		paqueteEnviar.tamanio=sizeof(int);

	     		enviar(nucleo, 101, paqueteEnviar.tamanio, paqueteEnviar.data); //segundo parametro cambiar cod de op
	     	}

	     	 if(paquete->codigo_operacion==108){ //no oficial codigo operacion

	     	paquete =recibir(nucleo);

	     	}

	     	liberar_paquete(paquete); //hacer los cases....

	 	 }

return 0;
}




