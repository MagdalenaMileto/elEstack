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

int cantPagsOcupadas;
pagina paginasSWAP[2000];
proceso procesos[100];
int contadorProcesos;

void *discoParaleloNoVirtualMappeado;

#define FALLORESERVARMEMORIA 15;
#define EXITORESERVARMEMORIA 10;
#define PROCESOLIBERADO 11;
#define EXITOLECTURA 12;
#define FALLOLECTURA 16;
#define EXITOESCRITURA 13;
#define FALLOESCRITURA 17;


int main(int argc,char **argv) {
	int sock_lst;
	int new_lst;
	void *paquetin;
	int pid, pagina;
	int tamanio_codigo;

	//pthread_t mock;
	//pthread_create(&mock, NULL, hilo_mock, NULL);

	if(abrirConfiguracion() == -1){return -1;};

	crearArchivo();
	mapearArchivo();

	//strncpy(discoParaleloNoVirtualMappeado, "HOLA MICA MICA HOLA",17);
	//memcpy(discoParaleloNoVirtualMappeado, "HOLA PUTO PUTO PUTO", 16);
	sock_lst = socket_escucha("localhost", PUERTO_SWAP);
	listen(sock_lst, 1024);

	while(1){

		new_lst = aceptar_conexion(sock_lst);
		printf("acepto\n");
		t_paquete *mensaje;

		while(1){
			mensaje = recibir(new_lst);
			int flagRespuesta;
			printf("*a\n");
			printf("Codigo operacion %d\n",mensaje->codigo_operacion);

			switch (mensaje->codigo_operacion) {
			case 1: {//Nuevo proceso
				//Deserializar Mensaje
				memcpy(&pid, mensaje->data, sizeof(int));
				memcpy(&pagina, mensaje->data + sizeof(int), sizeof(int));
				paquetin = malloc(pagina * TAMANIO_PAGINA);
				memcpy(paquetin, mensaje->data + sizeof(int) * 2, TAMANIO_PAGINA * pagina);
				printf("%s, %d %d \n",(char*)paquetin, pid, pagina);

				printf("Se creara un nuevo proceso de %d paginas y con PID: %d \n", pagina, pid);
				sleep(RETARDO_ACCESO);

				flagRespuesta = FALLORESERVARMEMORIA;
				int espacio = hayLugarParaNuevoProceso(pagina);

				if(espacio == -2){
					compactacion();
					int espacio2 = hayLugarParaNuevoProceso(pagina);
					flagRespuesta = reservarProceso(pid, pagina, espacio2);
				}
				if(espacio == -1)
				{
					flagRespuesta = -1;//No se puede asignar
				}
				else{
					flagRespuesta = reservarProceso(pid, pagina, espacio);
				}

				if (flagRespuesta == 1){
					int i, error;
					for(i=0;i<pagina;i++){
						void *dataPaginaAEscribir;
						dataPaginaAEscribir = malloc(TAMANIO_PAGINA);
						memcpy(dataPaginaAEscribir, paquetin + (i*TAMANIO_PAGINA), TAMANIO_PAGINA);
						error = escribirPaginaProceso(pid, i, dataPaginaAEscribir);
						free(dataPaginaAEscribir);
						if (error == -1){
							flagRespuesta = FALLORESERVARMEMORIA;
							break;
						}
					}
				}

				enviar(new_lst, flagRespuesta, sizeof(int), &pid);
				free(paquetin);

				break;
			}
				case 2: { //caso de Sacar proceso
					//Deserializar Mensaje
					memcpy(&pid, mensaje->data, sizeof(int));
					printf("Se liberara el proceso %d \n", pid);
					sleep(RETARDO_ACCESO);
					flagRespuesta = liberarProceso(pid);
					break;
				}
				case 4: { //caso de Escritura en disco //SIEMPRE ME LLEGA CON TAMANIO PAGINA
					//Deserializar Mensaje
					memcpy(&pid, mensaje->data, sizeof(int));
					memcpy(&pagina, mensaje->data + sizeof(int), sizeof(int));
					memcpy(&tamanio_codigo, mensaje->data + sizeof(int) * 2, sizeof(int));
					paquetin = malloc(pagina * TAMANIO_PAGINA);
					memcpy(paquetin, mensaje->data + sizeof(int) * 2, TAMANIO_PAGINA * pagina);

					printf("Se escribira para el proceso %d \n", pid);
					sleep(RETARDO_ACCESO);
					flagRespuesta = escribirPaginaProceso(pid, pagina, paquetin);
					if (flagRespuesta == 1){
						flagRespuesta = EXITOESCRITURA;
					}
					else{
						flagRespuesta = FALLOESCRITURA;
					}
					free(paquetin);

					break;
				}

				case 3: { //caso de Lectura de pagina
					void *paginaALeer;
					paginaALeer = malloc(TAMANIO_PAGINA);

					memcpy(&pid, mensaje->data, sizeof(int));
					memcpy(&pagina, mensaje->data + sizeof(int), sizeof(int));

					printf("Se leera la pagina: %d, del proceso %d \n", pagina, pid);
					sleep(RETARDO_ACCESO);
					flagRespuesta = leerPaginaProceso(pid, pagina, paginaALeer);
//					if (flagRespuesta == FALLOLECTURA){
//						return -1;
//					}
					enviar(new_lst, flagRespuesta, TAMANIO_PAGINA, paginaALeer);

					free(paginaALeer);
					break;
				}
				default: printf("Pedido incorrecto");
			}
		}
	}
	return 0;
}

int abrirConfiguracion() {
	int error = 0;
	configuracion = config_create("config_swap.ini");
	printf("Cargando el archivo de configuracion. \n");
	if (configuracion->properties->elements_amount <= 0) {
		perror("No se pudo cargar el archivo de configuración.");
		return -1;
	}

	if(config_has_property(configuracion, "PUERTO_ESCUCHA")){
		PUERTO_SWAP = config_get_string_value(configuracion, "PUERTO_ESCUCHA");
		printf("El puerto habilitado es el %s \n", PUERTO_SWAP);
	}else{
		perror("No se encontro el puerto en el archivo de configuracion");
		error = 1;
	}

	if(config_has_property(configuracion, "NOMBRE_SWAP")){
		NOMBRE_SWAP = config_get_string_value(configuracion, "NOMBRE_SWAP");
		printf("El archivo en disco se llama: %s \n", NOMBRE_SWAP);
	}else{
		perror("No se encontro el nombre del archivo en disco en el archivo de configuracion");
		error = 1;
	}

	if(config_has_property(configuracion, "CANTIDAD_PAGINAS")){
		CANTIDAD_PAGINAS = config_get_int_value(configuracion, "CANTIDAD_PAGINAS");
		printf("El archivo en disco contiene %d paginas\n", CANTIDAD_PAGINAS);
	}else{
		perror("No se encontro la cantidad de paginas en el archivo de configuracion");
		error = 1;
	}
	if(config_has_property(configuracion, "TAMANIO_PAGINA")){
		TAMANIO_PAGINA = config_get_int_value(configuracion, "TAMANIO_PAGINA");
		printf("El tamanio de cada pagina es de %d \n", TAMANIO_PAGINA);
	}else{
		perror("No se encontro el tamanio de paginas en el archivo de configuracion");
		error=1;
	}
	if(config_has_property(configuracion, "RETARDO_COMPACTACION")){
		RETARDO_COMPACTACION = config_get_int_value(configuracion, "RETARDO_COMPACTACION");
		printf("El tiempo de retardo en compactacion, es de: %d \n", RETARDO_COMPACTACION);
	}else{
		perror("No se encontro el retardo de compactacion en el archivo de configuracion");
		error=1;
	}
	if(config_has_property(configuracion, "RETARDO_ACCESO")){
		RETARDO_ACCESO = config_get_int_value(configuracion, "RETARDO_ACCESO");
		printf("El tiempo de retardo en el acceso, es de: %d \n", RETARDO_ACCESO);
	}else{
		perror("No se encontro el retardo de acceso en el archivo de configuracion");
		error=1;
	}
	if(error){
		return -1;

	}else{
		return 0;
	}
}

int crearArchivo(){
	NOMBRE_SWAP = config_get_string_value(configuracion, "NOMBRE_SWAP");
	printf("El archivo en disco se llama: %s \n", NOMBRE_SWAP);
	FILE *arch = fopen(NOMBRE_SWAP,"w");
	tamanio_archivo = CANTIDAD_PAGINAS*TAMANIO_PAGINA;
	void *archivo;
	archivo = malloc(tamanio_archivo);
	memset(archivo, '\0', tamanio_archivo);
	fwrite(archivo,1,tamanio_archivo,arch);
	fclose(arch);
	inicializarEstructuraPaginas();
	contadorProcesos = 0;

	return 1;
}

int mapearArchivo(){
	int fd;
	size_t length = tamanio_archivo;
	//discoParaleloNoVirtualMappeado = malloc(tamanio_archivo);

	fd = open(NOMBRE_SWAP, O_RDWR);
	if (fd == -1)
	{
		printf("Error abriendo %s para su lectura", NOMBRE_SWAP);
		exit(EXIT_FAILURE);
	}
	discoParaleloNoVirtualMappeado = mmap(0, length,PROT_READ |PROT_WRITE, MAP_SHARED,fd,0);
	//check(discoParaleloNoVirtualMappeado == MAP_FAILED, "mmap %s failed: %s", NOMBRE_SWAP, strerror (errno));

	if (discoParaleloNoVirtualMappeado == MAP_FAILED)
	{
		close(fd);
		printf("Error mapeando el archivo %s \n", NOMBRE_SWAP);
		exit(EXIT_FAILURE);
	}
	printf("Mapeo perfecto  %s \n", NOMBRE_SWAP);
	return 1;
}

void check (int test, const char * message, ...)
{
    if (test) {
        va_list args;
        va_start (args, message);
        vfprintf (stderr, message, args);
        va_end (args);
        fprintf (stderr, "\n");
        exit (EXIT_FAILURE);
    }
}


int hayLugarParaNuevoProceso(int cantPagsNecesita){
	//en caso de no encontrar lugar, me devuelve -1 y paso a hacer la compactacion
	//en otro caso, me devuelve el numero de pag a partir de la cual escribo
	int flagPuede = -1;
	int j=ultimaPagLibre();
	int i = j;
	int totalLibres = 0;
	while(1){
		if(j == -1){
			//no hay paginas libres
			break;
		}
		int primerOcupada = primerPaginaOcupadaLuegoDeUnaLibreDada(j);
		if((primerOcupada - j) >= cantPagsNecesita || primerOcupada == -1)
		{
			return j;
		}
		else {
			totalLibres = totalLibres + primerOcupada-j;
			i = j;


			if(i == CANTIDAD_PAGINAS -1){
				return -1;
			}

			//Hay que compactar para que pueda entrar el proceso
			if(totalLibres >=cantPagsNecesita)
			{
				return -2;
			}
			int k = j;
			j = primerPagLibreAPartirDeUnaDada(k);
		}
	}
	return flagPuede;
}


int primerPaginaOcupadaLuegoDeUnaLibreDada(int nroPag){
	int nroPaginaOcupada = -1;
	int estadoPaginaSiguiente = -1;
	int i;
	for(i=nroPag+1;i<CANTIDAD_PAGINAS;i++){
		estadoPaginaSiguiente = paginasSWAP[i].ocupada;
		if(estadoPaginaSiguiente == 1)
		{
			return (nroPaginaOcupada = i);
		}
	}
return nroPaginaOcupada;
}

int primerPagLibreAPartirDeUnaDada(int numeroPag){
	int primerPagLibre;
	int i;
	for(i=numeroPag;i<CANTIDAD_PAGINAS;i++){
		if(paginasSWAP[i].ocupada ==0)
		{
			return primerPagLibre= i;
		}
	}
	return -1;
}


int ultimaPagLibre(){
	int primerPagLibre;
	int i;
	for(i=0;i<CANTIDAD_PAGINAS;i++){
		if(paginasSWAP[i].ocupada ==0){
			return primerPagLibre= i;
		}
	}
	return -1;
}

int reservarProceso(int pidProceso, int cantPags , int pagAPartir){
	int error;
	error = asignarEspacio(cantPags, pidProceso, pagAPartir);
	if(error == -1){
		printf("Hubo error al asignar el proceso %d \n", pidProceso);
		return -1;
	}
	printf("Se reservo el espacio para el proceso con PID: %d de %d paginas \n", pidProceso, cantPags);
	return 1;
}

int asignarEspacio(int cantPagsAAsignar, int proceso, int inicio){
	int primerPaginaLibre = inicio;
	int i;
	if(inicio +cantPagsAAsignar > CANTIDAD_PAGINAS)
	{
		return -1;
	}
	for(i=0;i<cantPagsAAsignar;i++){
		printf("Se asignara la pagina %d al proceso %d \n", primerPaginaLibre+i,proceso);
		paginasSWAP[primerPaginaLibre +i].ocupada = 1;
		paginasSWAP[primerPaginaLibre +i].idProcesoQueLoOcupa = proceso;
		cantPagsOcupadas++;
	}
	procesos[contadorProcesos].idProceso = proceso;
	procesos[contadorProcesos].cantPagsUsando = cantPagsAAsignar;
	contadorProcesos++;

	return 1;
}

int compactacion(){ //Flag: 1 salio bien, 2 hubo error
	printf("SE INVOCO AL MODULO DE COMPACTACION... \n");
	printf("INICIANDO COMPACTACION \n");
	int resultado = -1;
	long int inicioOcupada, finOcupada;
	long int inicioLibre, finLibre;
	int primerPaginaOcupada;
	int primerPaginaLibre;
	do {
		primerPaginaLibre = ultimaPagLibre();
		primerPaginaOcupada = primerPaginaOcupadaLuegoDeUnaLibre();
		if(primerPaginaOcupada == -1)
		{
			printf("No hay mas paginas que compactar, finalizando correctamente. \n");
			break;
		}
		leerPagina(primerPaginaOcupada, &inicioOcupada, &finOcupada);
		leerPagina(primerPaginaLibre,&inicioLibre,&finLibre);
		printf("La pagina ocupada %d pasara a la pagina %d libre \n", primerPaginaOcupada, primerPaginaLibre);
		memcpy(discoParaleloNoVirtualMappeado + inicioLibre, discoParaleloNoVirtualMappeado + inicioOcupada, TAMANIO_PAGINA);
		//memset(discoParaleloNoVirtualMappeado + inicioLibre, &discoParaleloNoVirtualMappeado + inicioOcupada, TAMANIO_PAGINA);
		paginasSWAP[primerPaginaOcupada].ocupada = 0;
		paginasSWAP[primerPaginaLibre].idProcesoQueLoOcupa = paginasSWAP[primerPaginaOcupada].idProcesoQueLoOcupa;
		paginasSWAP[primerPaginaOcupada].idProcesoQueLoOcupa = -1;
		paginasSWAP[primerPaginaLibre].ocupada = 1;
	}
	while (hayPaginasOcupadasLuegoDeLaUltimaLibre());
	///updatearArchivoDisco();
	resultado = 1;
	printf("Terminando compactacion...\n");
	sleep(RETARDO_COMPACTACION);
	return resultado;
}

int primerPaginaOcupadaLuegoDeUnaLibre(){
	int estadoPaginaSiguiente = -1;
	int estadoPaginaAnterior =-1;
	int i;
	for(i=0;i<CANTIDAD_PAGINAS;i++){
		estadoPaginaAnterior = paginasSWAP[i].ocupada;
		estadoPaginaSiguiente = paginasSWAP[i+1].ocupada;
		if(estadoPaginaAnterior == 0)
		{
			if(estadoPaginaSiguiente == 1)
			{
				return (i+1);
			}
		}
	}
	return -1;
}


int leerPagina( int nroPag, long int*inicio, long int*fin){
	*inicio = nroPag*TAMANIO_PAGINA;
	if(*inicio >tamanio_archivo)
	{
		printf("No se puede leer la pagina \n");
		return -1;
	}
	*fin = *inicio +TAMANIO_PAGINA-1;
	return 1;
}

int hayPaginasOcupadasLuegoDeLaUltimaLibre(){
	int i;
	int flagExistencia;
	for(i=ultimaPagLibre();i<CANTIDAD_PAGINAS;i++){
		if(paginasSWAP[i].ocupada == 1)
		{
			flagExistencia=1;
		}
	}
	return flagExistencia;
}

//int updatearArchivoDisco(){
//	FILE* archendisco =fopen(NOMBRE_SWAP, "w");;
//	int i =0;
//	printf("Escribiendo en %s, archivo en disco \n", NOMBRE_SWAP);
//	for(i=0;i<tamanio_archivo; i++)
//	{
//		//TODO no estoy seguro de esto.
//		fputc((int)discoParaleloNoVirtualMappeado + i,archendisco);
//	}
//	fclose(archendisco);
//	mapearArchivo();
//	return 1;
//}

int liberarProceso(int idProc){
	int posProcEnElArray;
	int cantPagsQueUsa;
	int primerPaginaDelProceso;
	posProcEnElArray = getPosicionDelProceso(idProc);
	cantPagsQueUsa = procesos[posProcEnElArray].cantPagsUsando;
	primerPaginaDelProceso = getPrimerPagProc(idProc);
	int i;
	printf("Liberando el proceso %d \n", idProc);
	for(i=0;i<cantPagsQueUsa;i++)
	{
		printf("Borrando pagina %d ocupada por %d \n", primerPaginaDelProceso, idProc);
		paginasSWAP[primerPaginaDelProceso].ocupada = 0;
		paginasSWAP[primerPaginaDelProceso].idProcesoQueLoOcupa = -1;
		primerPaginaDelProceso++; //aumento en 1
		cantPagsOcupadas--;
	}

	printf("Se libero el proceso %d \n", idProc);
	return PROCESOLIBERADO;
}

int getPrimerPagProc(int idProceso){
	int pag;
	int i;
	for(i=0;i<CANTIDAD_PAGINAS;i++){
		if(paginasSWAP[i].idProcesoQueLoOcupa == idProceso)
		{
			return pag = i;
		}
	}
	return -1;
}

int getPosicionDelProceso(int idProc){
	int idProceso = -1;
	int i;
	for(i=0;i<contadorProcesos; i++){
		if(procesos[i].idProceso == idProc){
			idProceso = i;
		}
	}
	return idProceso;
}

int escribirPaginaProceso(int idProceso, int nroPag, void*data){
	int resultado;
	int posProc = getPosicionDelProceso(idProceso);
	int primeraPagProceso = getPrimerPagProc(idProceso);
	int cantidadPagsQueUsa = procesos[posProc].cantPagsUsando;
	int paginaAEscribir = nroPag + primeraPagProceso;
	if(nroPag > cantidadPagsQueUsa)
	{
		printf("No puede escribir en %d, no esta en su rango \n", nroPag);
		return -1;
	}
	printf("Se escribira la pagina %d del proceso: %d, cuya asociada es %d \n", nroPag,idProceso,paginaAEscribir);
	resultado = escribirPagina(paginaAEscribir, data);
	return resultado;
}

int escribirPagina(int nroPag, void*dataPagina){
	long int inicioPag;
	long int finPag;

	printf("Se escribira la pagina %d \n",nroPag);
	int numero = nroPag;
	leerPagina(numero, &inicioPag, &finPag); //con esto determino los valores de inicio de escritura y de fin
	memset(discoParaleloNoVirtualMappeado +inicioPag, '\0', TAMANIO_PAGINA);
	memcpy(discoParaleloNoVirtualMappeado +inicioPag, dataPagina, TAMANIO_PAGINA);
	printf("Pagina %d, copiada con exito! \n", nroPag);
//	if(updatearArchivoDisco()==1)
//	{
//		printf("Pagina %d, copiada con exito! \n", nroPag);
//	}
	return 1;
}

int leerPaginaProceso(int idProceso, int nroPag, void* paginaALeer){
	int flagResult;
	int posProc = getPosicionDelProceso(idProceso);
	int primerPag=getPrimerPagProc(idProceso);
	int cantPagsEnUso = procesos[posProc].cantPagsUsando;
	int posPag = primerPag +nroPag;
	if(nroPag>cantPagsEnUso)
	{
		printf("No puede leer la pagina %d, no tiene permisos \n",nroPag);
		return -1;
	}
	long int inicio,fin;
	flagResult = leerPagina(posPag,&inicio,&fin); //con esto determino los valores de inicio de lectura y de fin
	if (flagResult == -1){
		return -1;
	}

	memcpy(&paginaALeer, discoParaleloNoVirtualMappeado + inicio, TAMANIO_PAGINA);
	return 1;
}

int inicializarEstructuraPaginas(){
	int i;
	for(i=0;i<CANTIDAD_PAGINAS;i++)
	{
		paginasSWAP[i].idProcesoQueLoOcupa = -1;
		paginasSWAP[i].ocupada = 0;
	}
	return 1;
}

void *hilo_mock(void *arg){
	char* codigo;
	codigo = malloc(TAMANIO_PAGINA);
//	int i;
//	for(i = 0; i<TAMANIO_PAGINA; i++){
//		codigo[i] = "a";
//	}
	codigo = "asdksksklsasdksksklsasdksksklsasdksls";
	printf("%s \n", codigo);
	sleep(4);
	int socket;
	socket = conectar_a("localhost", "1209");
	printf("conecte %d\n",socket);

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
	sleep(1);
	printf("envie\n");
	//enviar(socket, 1,TAMANIO_PAGINA +2*sizeof(int), puntero);
	enviar(socket, 1, tamanio_paquete, data);


	printf("33\n");

	while(1){
		// recive




	};
}

