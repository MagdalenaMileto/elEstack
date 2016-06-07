/*
 * swap.c
 *
 *  Created on: 21/4/2016
 *      Author: Ivan Bober
 */

#include "swap.h"

int cantPagsOcupadas;
pagina paginasSWAP[2000];
proceso procesos[100];
int contadorProcesos;

char * discoSwapMapped;
char* discoMapped;
char * discoParaleloNoVirtual;
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
	char * codigo;
	int pid, pagina, tamanio_codigo;

	if(abrirConfiguracion() == -1){return -1;};

	crearArchivo();
	mapearArchivo();

	sock_lst = socket_escucha("localhost", PUERTO_SWAP);
	listen(sock_lst, 1024);


	while(1){
		new_lst = aceptar_conexion(sock_lst);
		t_paquete *mensaje;

		while(1){
			mensaje = recibir(new_lst);
			int flagRespuesta;

			switch (mensaje->codigo_operacion) {
			case 0: {//Nuevo proceso
				//Deserializar Mensaje
				memcpy(&pid, mensaje->data, sizeof(int));
				memcpy(&pagina, mensaje->data + sizeof(int), sizeof(int));
				memcpy(&tamanio_codigo, mensaje->data + sizeof(int) * 2, sizeof(int));
				codigo = malloc(tamanio_codigo);
				memcpy(codigo, mensaje->data + sizeof(int) * 3, tamanio_codigo);

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
					flagRespuesta = FALLORESERVARMEMORIA;//No se puede asignar
				}
				else{
					flagRespuesta = reservarProceso(pid, pagina, espacio);
				}

				//No deberia ser otro valor en vez de 4?
				enviar(new_lst, flagRespuesta, 4, &pid);
				free(codigo);

				break;
			}
				case 1: { //caso de Sacar proceso
					//Deserializar Mensaje
					memcpy(&pid, mensaje->data, sizeof(int));

					printf("Se liberara el proceso %d \n", pid);
					sleep(RETARDO_ACCESO);

					flagRespuesta = liberarProceso(pid);

					//Creo que no es necesario enviar.
					enviar(new_lst, flagRespuesta, 4, (void *) &pid);
					break;
				}
				case 2: { //caso de Escritura en disco
					//Deserializar Mensaje
					memcpy(&pid, mensaje->data, sizeof(int));
					memcpy(&pagina, mensaje->data + sizeof(int), sizeof(int));
					memcpy(&tamanio_codigo, mensaje->data + sizeof(int) * 2, sizeof(int));
					codigo = malloc(tamanio_codigo);
					memcpy(codigo, mensaje->data + sizeof(int) * 3, tamanio_codigo);

					printf("Se escribira para el proceso %d \n", pid);
					sleep(RETARDO_ACCESO);
					flagRespuesta = escribirPaginaProceso(pid, pagina, codigo);
					enviar(new_lst, flagRespuesta, 4, (void*)&pid);

					break;
				}

				case 3: { //caso de Lectura de pagina
					char *paginaALeer;
					paginaALeer = malloc(TAMANIO_PAGINA);

					memcpy(&pid, mensaje->data, sizeof(int));
					memcpy(&pagina, mensaje->data + sizeof(int), sizeof(int));

					printf("Se leera la pagina: %d, del proceso %d \n", pagina, pid);
					sleep(RETARDO_ACCESO);
					flagRespuesta = leerPaginaProceso(pid, pagina, paginaALeer);
//					if (flagRespuesta == FALLOLECTURA){
//						return -1;
//					}

					int tamanio_codigo = string_length(paginaALeer);
					void * data = malloc(tamanio_codigo);
					memcpy(data, paginaALeer, tamanio_codigo);
					enviar(new_lst, flagRespuesta, tamanio_codigo, data);
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
	char *vectorArchivo[tamanio_archivo];
	int i;
	char* caracter = "\0";
	for(i=0;i<tamanio_archivo;i++)
	{
		vectorArchivo[i] = caracter;
	}
	for(i=0;i<tamanio_archivo;i++)
	{
		fwrite(vectorArchivo[i],1,1,arch);
	}

	fclose(arch);
	inicializarEstructuraPaginas();
	contadorProcesos = 0;

	return 1;
}

int mapearArchivo(){
	int fd;
	size_t length = tamanio_archivo;
	discoSwapMapped=malloc(tamanio_archivo);
	discoParaleloNoVirtual = malloc(tamanio_archivo);
	fd = open(NOMBRE_SWAP, O_RDONLY);
	  if (fd == -1)
	  {
		printf("Error abriendo %s para su lectura", NOMBRE_SWAP);
		exit(EXIT_FAILURE);
	    }
	discoSwapMapped = mmap(0, length,PROT_READ, MAP_SHARED,fd,0);
	if (discoSwapMapped == MAP_FAILED)
	{
		close(fd);
		printf("Error mapeando el archivo %s \n", NOMBRE_SWAP);
		exit(EXIT_FAILURE);
	}
	discoMapped = discoSwapMapped;
	int i;
		for(i=0;i<tamanio_archivo;i++){
			discoParaleloNoVirtual[i] = discoMapped[i];
		}
	printf("Mapeo perfecto  %s \n", NOMBRE_SWAP);
	return 1;
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
		return FALLORESERVARMEMORIA;
	}
	printf("Se reservo el espacio para el proceso con PID: %d de %d paginas \n", pidProceso, cantPags);
	return EXITORESERVARMEMORIA;
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
	int i;
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
		for(i=0;i<TAMANIO_PAGINA;i++)
		{
			char car = discoParaleloNoVirtual[(inicioOcupada+i)];;
			discoParaleloNoVirtual[(inicioLibre+i)] = car;}
			paginasSWAP[primerPaginaOcupada].ocupada = 0;
			paginasSWAP[primerPaginaLibre].idProcesoQueLoOcupa = paginasSWAP[primerPaginaOcupada].idProcesoQueLoOcupa;
			paginasSWAP[primerPaginaOcupada].idProcesoQueLoOcupa = -1;
			paginasSWAP[primerPaginaLibre].ocupada = 1;
	}
	while (hayPaginasOcupadasLuegoDeLaUltimaLibre());
	updatearArchivoDisco();
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

int updatearArchivoDisco(){
	FILE* archendisco =fopen(NOMBRE_SWAP, "w");;
	int i =0;
	printf("Escribiendo en %s, archivo en disco \n", NOMBRE_SWAP);
	for(i=0;i<tamanio_archivo; i++)
	{
		fputc(discoParaleloNoVirtual[i],archendisco);
	}
	fclose(archendisco);
	mapearArchivo();
	return 1;
}

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

int escribirPaginaProceso(int idProceso, int nroPag, char*data){
	int resultado;
	int posProc = getPosicionDelProceso(idProceso);
	int primeraPagProceso = getPrimerPagProc(idProceso);
	int cantidadPagsQueUsa = procesos[posProc].cantPagsUsando;
	int paginaAEscribir = nroPag + primeraPagProceso;
	if(nroPag > cantidadPagsQueUsa)
	{
		printf("No puede escribir en %d, no esta en su rango \n", nroPag);
		return FALLOESCRITURA;
	}
	printf("Se escribira la pagina %d del proceso: %d, cuya asociada es %d \n", nroPag,idProceso,paginaAEscribir);
	resultado = escribirPagina(paginaAEscribir, data);
	return resultado;
}

int escribirPagina(int nroPag, char*dataPagina){
	long int inicioPag;
	long int finPag;
	int cantCaracteres;
	cantCaracteres = strlen(dataPagina);

	printf("Se escribira la pagina %d de %d bytes, aun asi ocupara %d \n",nroPag ,cantCaracteres,TAMANIO_PAGINA);
	int numero = nroPag;
	leerPagina(numero, &inicioPag, &finPag); //con esto determino los valores de inicio de escritura y de fin
	int i=0;
	memset(discoParaleloNoVirtual, '\0', TAMANIO_PAGINA);
	for(i =0; i<cantCaracteres;i++){
		char car = dataPagina[i];
		discoParaleloNoVirtual[(inicioPag+i)] = car;
	}
	if(updatearArchivoDisco(numero)==1)
	{
		printf("Pagina %d, copiada con exito! Contenido de la misma: %s \n", nroPag, dataPagina);
	}
	return EXITOESCRITURA;
}

int leerPaginaProceso(int idProceso, int nroPag, char* paginaALeer){
	int flagResult;
	int posProc = getPosicionDelProceso(idProceso);
	int primerPag=getPrimerPagProc(idProceso);
	int cantPagsEnUso = procesos[posProc].cantPagsUsando;
	int posPag = primerPag +nroPag;
	if(nroPag>cantPagsEnUso)
	{
		printf("No puede leer la pagina %d, no tiene permisos \n",nroPag);
		return FALLOLECTURA;
	}
	long int inicio,fin;
	flagResult = leerPagina(posPag,&inicio,&fin); //con esto determino los valores de inicio de lectura y de fin
	if (flagResult == -1){
		return FALLOLECTURA;
	}
	int i;
	char*dataLeida = malloc(TAMANIO_PAGINA);
	for(i=0;i<TAMANIO_PAGINA; i++)
	{
		dataLeida[i]= discoMapped[inicio +i];
	}
	strcpy(paginaALeer,dataLeida);
	return EXITOLECTURA;
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

