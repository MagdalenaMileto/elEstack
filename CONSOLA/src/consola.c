/*
 * consola.c
 *
 *  Created on: 23/4/2016
 *      Author: utnso
 */


#include "funcionesConsola.h"

int socketConexionNucleo;
int programa_finalizado=0;


int main(int argc, char **argv) {

	char* script;
	FILE *archivo;
	char nomArchivo[50];
	signed int nucleo;

	archivoDeConfiguracion(argv[0]);
	strcpy(nomArchivo,argv[1]);
	archivo = fopen(nomArchivo, "r");

	if (archivo == NULL){
			printf("No se pudo acceder al script.\n");
			return EXIT_FAILURE;
	}
	else{
			script = leerElArchivo(archivo);
			fclose(archivo);
			printf("Ejecutando programa ansisop:\n");
			printf("El script: %s\n", script);
	}

	char* script_enviar= malloc(strlen(script));
	memcpy(script_enviar, script, strlen(script));
	nucleo= conectarConElNucleo();
	enviar(nucleo, 103, script_enviar, strlen(script_enviar));
	free(script_enviar);


t_paquete *paquete;
char* info_cadena;
int info_variable;
int codigo_op;

while(!programa_finalizado){
		paquete = recibir(nucleo);
		memcpy(&codigo_op, paquete->codigo_operacion, 4);


		switch(codigo_op) {
		case 161:
				 memcpy(info_cadena, paquete->data, paquete->tamanio);
				 puts(info_cadena);
				 break;
		case 160:
				memcpy(&info_variable, paquete->data, 4);
				printf("%d\n", info_variable);
				break;
		case 162:
				puts("Programa Finalizado\n");
				programa_finalizado=1;
				close(nucleo);
				break;
		}

}


liberar_paquete(paquete);
printf("CONSOLA: Cierro\n");

return 0;
}



//********************************************FUNCIONES********************************************


void archivoDeConfiguracion(char * pathconf) {

	char archi[15] = "/consola.confg";
	char * archivo = malloc(15+ strlen(pathconf));

	memcpy(archivo + strlen(pathconf),pathconf, strlen(pathconf));
	memcpy(archivo, archi, 15);

	printf("%s\n", pathconf);
	t_config * archivo_configuracion = config_create(pathconf);

	puerto_nucleo = config_get_string_value(archivo_configuracion, "PUERTO_NUCLEO");
	ip_nucleo = config_get_string_value(archivo_configuracion, "IP_NUCLEO");

	free(archivo);
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



