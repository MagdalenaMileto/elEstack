/*
 * swap.c
 *
 *  Created on: 21/4/2016
 *      Author: Ivan Bober
 */

#include "swap.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

pagina paginasSWAP[2000];

int main(int argc, char **argv) {
	int sock_lst;
	int socket_umc;
	void *codigo;
	int pid, pagina;


	//pthread_t mock;
	//pthread_create(&mock, NULL, hilo_mock, NULL);

	if (abrirConfiguracion() == -1) {
		return -1;
	};

	crearArchivo();
	mapearArchivo();

	sock_lst = socket_escucha("localhost", PUERTO_SWAP);
	listen(sock_lst, 1024);

	while (1) {

		socket_umc = aceptar_conexion(sock_lst);
		printf("acepto\n");
		t_paquete *mensaje;

		while (1) {
			mensaje = recibir(socket_umc);

			//printf("*a\n");
			printf("Codigo operacion %d\n", mensaje->codigo_operacion);

			switch (mensaje->codigo_operacion) {
			case INICIALIZAR: 	//Nuevo proceso

				//Deserializar Mensaje
				memcpy(&pid, mensaje->data, sizeof(int));
				memcpy(&pagina, mensaje->data + sizeof(int), sizeof(int));

				int tamanio_codigo;
				tamanio_codigo = mensaje->tamanio - 2 * sizeof(int);
				codigo = malloc(TAMANIO_PAGINA * pagina);
				memset(codigo, '\0', TAMANIO_PAGINA * pagina);
				memcpy(codigo, mensaje->data + sizeof(int) * 2, tamanio_codigo);

				//printf("UMC: %s, %d %d \n",(char*)paquetin, pid, pagina);

				printf( "Se creara un nuevo proceso de %d paginas y con PID: %d \n", pagina, pid);
				usleep(RETARDO_ACCESO * 1000);

				int flagRespuesta;

				int paginaAPartir = hayLugarParaNuevoProceso(pagina);
				if(!paginaAPartir ==-1)
				{
					reservarProceso(pid, pagina, paginaAPartir);
					inicializarProceso(pid, pagina, codigo);
				}
				else
				{
					compactacion();
					int pagAPartir = hayLugarParaNuevoProceso(pagina);
					if(!pagAPartir ==-1){
						reservarProceso(pid, pagina, paginaAPartir);
						inicializarProceso(pid, pagina, codigo);
						flagRespuesta = EXITO;
					}
					else{
						flagRespuesta = NOHAYLUGAR;
					}
				}

				enviar(socket_umc, flagRespuesta, sizeof(int), &pid);
				free(codigo);
				break;

			case FINALIZAR: { //caso de Sacar proceso
				//Deserializar Mensaje
				memcpy(&pid, mensaje->data, sizeof(int));
				printf("Se liberara el proceso %d \n", pid);
				usleep(RETARDO_ACCESO * 1000);
				liberarProceso(pid);
				break;
			}
			case ESCRIBIR: { //caso de Escritura en disco //SIEMPRE ME LLEGA CON TAMANIO PAGINA
				//Deserializar Mensaje
				memcpy(&pid, mensaje->data, sizeof(int));
				memcpy(&pagina, mensaje->data + sizeof(int), sizeof(int));
				memcpy(&tamanio_codigo, mensaje->data + sizeof(int) * 2,
						sizeof(int));
				codigo = malloc(pagina * TAMANIO_PAGINA);
				memcpy(codigo, mensaje->data + sizeof(int) * 2,
						TAMANIO_PAGINA * pagina);

				printf("Se escribira para el proceso %d \n", pid);
				usleep(RETARDO_ACCESO * 1000);

				escribirPaginaProceso(pid, pagina, codigo);

				free(codigo);
				break;
			}

			case LEER: { //caso de Lectura de pagina
				void * paginaALeer;
				paginaALeer = malloc(TAMANIO_PAGINA);
				memset(paginaALeer, '\0',TAMANIO_PAGINA);

				memcpy(&pid, mensaje->data, sizeof(int));
				memcpy(&pagina, mensaje->data + sizeof(int), sizeof(int));

				printf("Se leera la pagina: %d, del proceso %d \n", pagina, pid);
				usleep(RETARDO_ACCESO * 1000);

				leerPaginaProceso(pid, pagina, paginaALeer);

				enviar(socket_umc, 1, TAMANIO_PAGINA, paginaALeer);

				break;
			}
			default:
				printf("Pedido incorrecto");
				exit(0);
			}
			liberar_paquete(mensaje);
		}
	}
	return 0;
}

void inicializarProceso(int pid, int pagina, char*codigo){
	int i;
	for(i=0;i<pagina;i++){
		void *dataPaginaAEscribir;
		dataPaginaAEscribir = malloc(TAMANIO_PAGINA);
		memcpy(dataPaginaAEscribir, codigo + (i*TAMANIO_PAGINA), TAMANIO_PAGINA);
		//printf("codigo  DIVIDO EN PAGINAS: %s \n", (char*)dataPaginaAEscribir);
		escribirPaginaProceso(pid, i, dataPaginaAEscribir);
		free(dataPaginaAEscribir);
	}
}

int abrirConfiguracion() {
	int error = 0;
	configuracion = config_create("config_swap.ini");
	printf("Cargando el archivo de configuracion. \n");
	if (configuracion->properties->elements_amount <= 0) {
		perror("No se pudo cargar el archivo de configuración.");
		return -1;
	}

	if (config_has_property(configuracion, "PUERTO_ESCUCHA")) {
		PUERTO_SWAP = config_get_string_value(configuracion, "PUERTO_ESCUCHA");
		printf("El puerto habilitado es el %s \n", PUERTO_SWAP);
	} else {
		perror("No se encontro el puerto en el archivo de configuracion");
		error = 1;
	}

	if (config_has_property(configuracion, "NOMBRE_SWAP")) {
		NOMBRE_SWAP = config_get_string_value(configuracion, "NOMBRE_SWAP");
		printf("El archivo en disco se llama: %s \n", NOMBRE_SWAP);
	} else {
		perror(
				"No se encontro el nombre del archivo en disco en el archivo de configuracion");
		error = 1;
	}

	if (config_has_property(configuracion, "CANTIDAD_PAGINAS")) {
		CANTIDAD_PAGINAS = config_get_int_value(configuracion,
				"CANTIDAD_PAGINAS");
		printf("El archivo en disco contiene %d paginas\n", CANTIDAD_PAGINAS);
	} else {
		perror(
				"No se encontro la cantidad de paginas en el archivo de configuracion");
		error = 1;
	}
	if (config_has_property(configuracion, "TAMANIO_PAGINA")) {
		TAMANIO_PAGINA = config_get_int_value(configuracion, "TAMANIO_PAGINA");
		printf("El tamanio de cada pagina es de %d \n", TAMANIO_PAGINA);
	} else {
		perror(
				"No se encontro el tamanio de paginas en el archivo de configuracion");
		error = 1;
	}
	if (config_has_property(configuracion, "RETARDO_COMPACTACION")) {
		RETARDO_COMPACTACION = config_get_int_value(configuracion,
				"RETARDO_COMPACTACION");
		printf("El tiempo de retardo en compactacion, es de: %d \n",
				RETARDO_COMPACTACION);
	} else {
		perror(
				"No se encontro el retardo de compactacion en el archivo de configuracion");
		error = 1;
	}
	if (config_has_property(configuracion, "RETARDO_ACCESO")) {
		RETARDO_ACCESO = config_get_int_value(configuracion, "RETARDO_ACCESO");
		printf("El tiempo de retardo en el acceso, es de: %d \n",
				RETARDO_ACCESO);
	} else {
		perror(
				"No se encontro el retardo de acceso en el archivo de configuracion");
		error = 1;
	}
	if (error) {
		return -1;

	} else {
		return 0;
	}
}

int crearArchivo() {
	NOMBRE_SWAP = config_get_string_value(configuracion, "NOMBRE_SWAP");
	printf("El archivo en disco se llama: %s \n", NOMBRE_SWAP);
	FILE *arch = fopen(NOMBRE_SWAP, "w");
	tamanio_archivo = CANTIDAD_PAGINAS * TAMANIO_PAGINA;
	void *archivo;
	archivo = malloc(tamanio_archivo);
	memset(archivo, '\0', tamanio_archivo);
	fwrite(archivo, 1, tamanio_archivo, arch);
	fclose(arch);
	//inicializarEstructuraPaginas();
	return 1;
}

int mapearArchivo() {
	int fd;
	size_t length = tamanio_archivo;
	//discoParaleloNoVirtualMappeado = malloc(tamanio_archivo);

	fd = open(NOMBRE_SWAP, O_RDWR);
	if (fd == -1) {
		printf("Error abriendo %s para su lectura", NOMBRE_SWAP);
		exit(EXIT_FAILURE);
	}
	discoParaleloNoVirtualMappeado = mmap(0, length, PROT_READ | PROT_WRITE,
	MAP_SHARED, fd, 0);
	//check(discoParaleloNoVirtualMappeado == MAP_FAILED, "mmap %s failed: %s", NOMBRE_SWAP, strerror (errno));

	if (discoParaleloNoVirtualMappeado == MAP_FAILED) {
		close(fd);
		printf("Error mapeando el archivo %s \n", NOMBRE_SWAP);
		exit(EXIT_FAILURE);
	}
	printf("Mapeo perfecto  %s \n", NOMBRE_SWAP);
	return 1;
}

void check(int test, const char * message, ...) {
	if (test) {
		va_list args;
		va_start(args, message);
		vfprintf(stderr, message, args);
		va_end(args);
		fprintf(stderr, "\n");
		exit(EXIT_FAILURE);
	}
}

int hayLugarParaNuevoProceso(int cantPagsNecesita) {
	int contadorPaginasSeguidas = 0;
	int i;
	int primeraPaginaLibre = 0;
	for (i = 0; i < CANTIDAD_PAGINAS; ++i) {
		if (paginasSWAP[i].ocupada == 0){
			if (contadorPaginasSeguidas == 0){
				primeraPaginaLibre = i;
			}
			contadorPaginasSeguidas++;
		}
		else {
			if (contadorPaginasSeguidas >= cantPagsNecesita){
				return primeraPaginaLibre;
			}
			else{
				contadorPaginasSeguidas = 0;
			}
		}

	}
	if (contadorPaginasSeguidas >= cantPagsNecesita){
		return primeraPaginaLibre;
	}
	else{
		return -1;
	}


//
//	//en caso de no encontrar lugar, me devuelve -1 y paso a hacer la compactacion
//	//en otro caso, me devuelve el numero de pag a partir de la cual escribo
//	int flagPuede = -1;
//	int j = ultimaPagLibre();
//	int i = j;
//	int totalLibres = 0;
//	while (1) {
//		if (j == -1) {
//			//no hay paginas libres
//			break;
//		}
//		int primerOcupada = primerPaginaOcupadaLuegoDeUnaLibreDada(j);
//		if ((primerOcupada - j) >= cantPagsNecesita || primerOcupada == -1) {
//			return j;
//		} else {
//			totalLibres = totalLibres + primerOcupada - j;
//			i = j;
//
//			if (i == CANTIDAD_PAGINAS - 1) {
//				return -1;
//			}
//
//			//Hay que compactar para que pueda entrar el proceso
//			if (totalLibres >= cantPagsNecesita) {
//				return -2;
//			}
//			int k = j;
//			j = primerPagLibreAPartirDeUnaDada(k);
//		}
//	}
//	return flagPuede;
}

int primerPaginaOcupadaLuegoDeUnaLibreDada(int nroPag) {
	int nroPaginaOcupada = -1;
	int estadoPaginaSiguiente = -1;
	int i;
	for (i = nroPag + 1; i < CANTIDAD_PAGINAS; i++) {
		estadoPaginaSiguiente = paginasSWAP[i].ocupada;
		if (estadoPaginaSiguiente == 1) {
			return (nroPaginaOcupada = i);
		}
	}
	return nroPaginaOcupada;
}

int primerPagLibreAPartirDeUnaDada(int numeroPag) {
	int primerPagLibre;
	int i;
	for (i = numeroPag; i < CANTIDAD_PAGINAS; i++) {
		if (paginasSWAP[i].ocupada == 0) {
			return primerPagLibre = i;
		}
	}
	return -1;
}

int ultimaPagLibre() {
	int primerPagLibre;
	int i;
	for (i = 0; i < CANTIDAD_PAGINAS; i++) {
		if (paginasSWAP[i].ocupada == 0) {
			return primerPagLibre = i;
		}
	}
	return -1;
}

int reservarProceso(int pidProceso, int cantPags, int pagAPartir) {
	int error;
	error = asignarEspacio(cantPags, pidProceso, pagAPartir);
	if (error == -1) {
		printf("Hubo error al asignar el proceso %d \n", pidProceso);
		return -1;
	}
	printf("Se reservo el espacio para el proceso con PID: %d de %d paginas \n",
			pidProceso, cantPags);
	return EXITO;
}

int asignarEspacio(int cantPagsAAsignar, int proceso, int inicio) {
	int primerPaginaLibre = inicio;
	int i;
	if (inicio + cantPagsAAsignar > CANTIDAD_PAGINAS) {
		return -1;
	}
	for (i = 0; i < cantPagsAAsignar; i++) {
		printf("Se asignara la pagina %d al proceso %d \n", primerPaginaLibre + i, proceso);
		paginasSWAP[primerPaginaLibre + i].ocupada = 1;
		paginasSWAP[primerPaginaLibre + i].idProcesoQueLoOcupa = proceso;
	}
	return 1;
}

int compactacion() { //Flag: 1 salio bien, 2 hubo error
	printf("SE INVOCO AL MODULO DE COMPACTACION... \n");
	printf("INICIANDO COMPACTACION \n");
	int resultado = -1;
	long int inicioOcupada;
	long int inicioLibre;
	int primerPaginaOcupada;
	int primerPaginaLibre;
	do {
		primerPaginaLibre = ultimaPagLibre();
		primerPaginaOcupada = primerPaginaOcupadaLuegoDeUnaLibre();
		if (primerPaginaOcupada == -1) {
			printf(
					"No hay mas paginas que compactar, finalizando correctamente. \n");
			break;
		}
		inicioOcupada = obtenerlugarDeInicioDeLaPagina(primerPaginaOcupada);
		inicioLibre = obtenerlugarDeInicioDeLaPagina(primerPaginaLibre);
		printf("La pagina ocupada %d pasara a la pagina %d libre \n", primerPaginaOcupada, primerPaginaLibre);
		memcpy(discoParaleloNoVirtualMappeado + inicioLibre, discoParaleloNoVirtualMappeado + inicioOcupada, TAMANIO_PAGINA);
		paginasSWAP[primerPaginaOcupada].ocupada = 0;
		paginasSWAP[primerPaginaLibre].idProcesoQueLoOcupa = paginasSWAP[primerPaginaOcupada].idProcesoQueLoOcupa;
		paginasSWAP[primerPaginaOcupada].idProcesoQueLoOcupa = -1;
		paginasSWAP[primerPaginaLibre].ocupada = 1;
	} while (hayPaginasOcupadasLuegoDeLaUltimaLibre());
	///updatearArchivoDisco();
	resultado = 1;
	printf("Terminando compactacion...\n");
	usleep(RETARDO_COMPACTACION * 1000);
	return resultado;
}

int primerPaginaOcupadaLuegoDeUnaLibre() {
	int estadoPaginaSiguiente = -1;
	int estadoPaginaAnterior = -1;
	int i;
	for (i = 0; i < CANTIDAD_PAGINAS; i++) {
		estadoPaginaAnterior = paginasSWAP[i].ocupada;
		estadoPaginaSiguiente = paginasSWAP[i + 1].ocupada;
		if (estadoPaginaAnterior == 0) {
			if (estadoPaginaSiguiente == 1) {
				return (i + 1);
			}
		}
	}
	return -1;
}

long int obtenerlugarDeInicioDeLaPagina(int nroPag) {
	return nroPag * TAMANIO_PAGINA;
}

int hayPaginasOcupadasLuegoDeLaUltimaLibre() {
	int i;
	int flagExistencia;
	for (i = ultimaPagLibre(); i < CANTIDAD_PAGINAS; i++) {
		if (paginasSWAP[i].ocupada == 1) {
			flagExistencia = 1;
		}
	}
	return flagExistencia;
}

void liberarProceso(int idProc) {
	int primerPaginaDelProceso;
	primerPaginaDelProceso = getPrimerPagProc(idProc);
	int i;
	printf("Liberando el proceso %d \n", idProc);
	for (i = 0; i < CANTIDAD_PAGINAS; i++) {
		if (paginasSWAP[i].idProcesoQueLoOcupa == idProc){
			printf("Borrando pagina %d ocupada por %d \n", primerPaginaDelProceso,idProc);

			paginasSWAP[primerPaginaDelProceso].ocupada = 0;
			paginasSWAP[primerPaginaDelProceso].idProcesoQueLoOcupa = -1;
			primerPaginaDelProceso++; //aumento en 1
		}
	}
	printf("Se libero el proceso %d \n", idProc);
}

int getPrimerPagProc(int idProceso) {
	int pag;
	int i;
	for (i = 0; i < CANTIDAD_PAGINAS; i++) {
		if (paginasSWAP[i].idProcesoQueLoOcupa == idProceso) {
			return pag = i;
		}
	}
	return -1;
}

void escribirPaginaProceso(int idProceso, int nroPag, void*data) {
	int primeraPagProceso = getPrimerPagProc(idProceso);
	int paginaAEscribir = nroPag + primeraPagProceso;

	printf("Se escribira la pagina %d del proceso: %d, cuya asociada es %d \n", nroPag, idProceso, paginaAEscribir);
	escribirPagina(paginaAEscribir, data);
}

void escribirPagina(int nroPag, void*dataPagina) {
	long int inicioPag;

	printf("Se escribira la pagina %d \n", nroPag);
	int numero = nroPag;
	inicioPag = obtenerlugarDeInicioDeLaPagina(numero); //con esto determino los valores de inicio de escritura
	memset(discoParaleloNoVirtualMappeado + inicioPag, '\0', TAMANIO_PAGINA);
	///printf("codigo: %s \n", (char*)dataPagina);

	printf("Pagina de Inicio en la que va a copiar:%d \n", inicioPag);
	memcpy(discoParaleloNoVirtualMappeado + inicioPag, dataPagina,
			TAMANIO_PAGINA);
	printf("Pagina %d, copiada con exito! \n", nroPag);
}

void leerPaginaProceso(int idProceso, int nroPag, void* paginaALeer) {
	int primerPag = getPrimerPagProc(idProceso);
	int posPag = primerPag + nroPag;

	//printf("codigo todo: %s \n", (char*) discoParaleloNoVirtualMappeado);

	long int inicio;
	inicio = obtenerlugarDeInicioDeLaPagina(posPag); //con esto determino los valores de inicio de lectura
	memcpy(paginaALeer, discoParaleloNoVirtualMappeado + inicio, TAMANIO_PAGINA);

	//printf("codigo: %s \n", (char*)paginaALeer);
}

int inicializarEstructuraPaginas() {
	int i;
	for (i = 0; i < CANTIDAD_PAGINAS; i++) {
		paginasSWAP[i].idProcesoQueLoOcupa = -1;
		paginasSWAP[i].ocupada = 0;
	}
	return 1;
}

/*
void *hilo_mock(void *arg) {
	char* codigo;
	codigo = malloc(TAMANIO_PAGINA);
//	int i;
//	for(i = 0; i<TAMANIO_PAGINA; i++){
//		codigo[i] = "a";
//	}
	codigo = "asdksksklsasdksksklsasdksksklsasdksls";
	printf("%s \n", codigo);
	usleep(4);
	int socket;
	socket = conectar_a("localhost", "1209");
	printf("conecte %d\n", socket);

	int pid = 1;
	int cantidad_paginas = 1;
	int tamanio_codigo = TAMANIO_PAGINA;
	int tamanio_paquete = (sizeof(int) * 2) + TAMANIO_PAGINA;
	void * data = malloc(tamanio_paquete);

	memcpy(data, &pid, sizeof(int));
	memcpy(data + sizeof(int), &cantidad_paginas, sizeof(int));
	memcpy(data + (sizeof(int) * 2), codigo, tamanio_codigo);

//	int *puntero = malloc(TAMANIO_PAGINA +2*sizeof(int));
//	printf("**\n");
//	puntero[0]=1; //pid
//	puntero[1]=1;
//	memcpy(puntero+2*sizeof(int),codigo, sizeof(codigo));
//	printf("**\n");
	usleep(1);
	printf("envie\n");
	//enviar(socket, 1,TAMANIO_PAGINA +2*sizeof(int), puntero);
	enviar(socket, 1, tamanio_paquete, data);

	printf("33\n");

	while (1) {
		// recive

	};
}
*/
