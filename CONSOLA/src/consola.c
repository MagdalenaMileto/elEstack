/*
 * consola.c
 *
 *  Created on: 23/4/2016
 *      Author: utnso
 */



#include "funcionesConsola.h"

int socketConexionNucleo;
int programa_finalizado=1;


int main(int argc, char **argv) {

	char* script;
	FILE *archivo;
	char nomArchivo[50];


	archivoDeConfiguracion(argv[0]);
	printf("pase archivo de configuracion\n");
	strcpy(nomArchivo,argv[1]);
	archivo = fopen(nomArchivo, "r");

	if (archivo == NULL){
			printf("No se pudo acceder al script\n");
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
	enviar(nucleo, 103, strlen(script), script_enviar);
	free(script_enviar);


t_paquete *paquete;
char* info_cadena;
int info_variable;


while(programa_finalizado){
		printf("entro al while\n");
		paquete = recibir(nucleo);
		printf("recibi algo\n");

		printf("tamanio:%d\n", paquete->tamanio);
		printf("codigo de op:%d\n", paquete->codigo_operacion);

		switch(paquete->codigo_operacion) {
		case 161:
				 memcpy(info_cadena, paquete->data, paquete->tamanio);
				 printf("cadena: %s\n",info_cadena);
				 break;
		case 160:
				printf("%d\n", *(int*)paquete->data);
				break;
		case 162:
				printf("Programa Finalizado\n");
				programa_finalizado=0;
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
	char * archivo = malloc(15+ strlen(pathconf)-7);

	memcpy(archivo ,pathconf, strlen(pathconf)-7);
	memcpy(archivo+ strlen(pathconf)-7, archi, 15);

	printf("%s\n", archivo);
	t_config * archivo_configuracion = config_create(archivo);

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
	printf("ip: %s puerto: %s\n", ip_nucleo, puerto_nucleo);
	nucleo= conectar_a(ip_nucleo, puerto_nucleo);

	if(nucleo==0){
		printf("CONSOLA: No encontre NUCLEO\n");
		exit (EXIT_FAILURE);
	}

	printf("CONSOLA: Conecta bien nucleo %d\n", nucleo);


        /*if(realizar_handshake(nucleo)){
            printf("CONSOLA:Handshake exitoso con nucleo\n");
        }else{
            printf("CONSOLA:Handshake invalido con nucleo\n");
        }*/

return nucleo;
}



