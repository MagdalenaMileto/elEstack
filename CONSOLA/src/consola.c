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
	t_paquete *paquete;
	char* info_cadena;


	archivoDeConfiguracion(argv[0]);
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



	while(programa_finalizado){
			paquete = recibir(nucleo);

			switch(paquete->codigo_operacion) {
			case 161:
					memcpy(&info_cadena, &paquete->data, paquete->tamanio);
					printf("Cadena: %s\n",info_cadena);
					break;
			case 160:
					printf("Valor: %d\n", *(int*)paquete->data);
					break;
			case 162:
					printf("Programa Finalizado\n");
					programa_finalizado=0;
					close(nucleo);
					break;

			case 163:
					printf("Programa abortado por SWAP sin espacio\n");
					programa_finalizado=0;
					close(nucleo);
				break;
			case 164:
					printf("Programa abortado por CPU\n");
					programa_finalizado=0;
					close(nucleo);
					break;
			}
}


liberar_paquete(paquete);
printf("Se cierra CONSOLA\n");

return 0;
}



//********************************************FUNCIONES********************************************


void archivoDeConfiguracion(char * pathconf) {

	char archi[14] = "consola.confg";
	char * archivo = malloc(14+ strlen(pathconf)-7);

	memcpy(archivo ,pathconf, strlen(pathconf)-7);
	memcpy(archivo+ strlen(pathconf)-7, archi, 14);

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
	nucleo= conectar_a(ip_nucleo, puerto_nucleo);

	if(nucleo==0){
		printf("CONSOLA: No encontre NUCLEO\n");
		exit (EXIT_FAILURE);
	}

	printf("CONSOLA: Conectado con nucleo\n");


return nucleo;
}



