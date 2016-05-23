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


int main(int argc,char **argv) {

	int sock_lst;
	int new_lst;
	int mensajeUmc;
	char buffer[100];
	int pedidoUMC;
	paqueteUMC* paquete = malloc(sizeof(paqueteUMC));

	abrirConfiguracion();
	crearArchivo();
	mapearArchivo();

	sock_lst = iniciarServidor();
	new_lst = establecerConexion(sock_lst);

	if ((mensajeUmc = recv(new_lst, buffer, sizeof(buffer), 0)) <= 0) {
		if (mensajeUmc == 0) {
			// UMC Cerro la conexion
			printf("SWAP: Select: La UMC %d se ha desconectado\n", new_lst);
			return EXIT_FAILURE;
		} else {
			//Hubo un error en la conexion
			perror("recv");
			return EXIT_FAILURE;
		}
	}

	paqueteUMC *paqueteRta= malloc(sizeof(paqueteUMC));

	//asociar a paquete con el proceso que me llega y el mensaje que entra que diga que se tiene que hacer (pedidoUMC)
	switch (pedidoUMC){
	case 0: {//Nuevo proceso
		// Tiene que haber retardo?
		printf("Se creara un nuevo proceso de %d paginas y con PID: %d \n", paquete->nroPag, paquete->pid);

		paqueteRta->pid = paquete->pid;
		int espacio = hayLugarParaNuevoProceso(paquete->nroPag);

		if(espacio == -2){
			compactacion();
			int espacio2 = hayLugarParaNuevoProceso(paquete->nroPag);
			paqueteRta->flagProc = reservarProceso(paquete->pid,paquete->nroPag, espacio2);
		}
		if(espacio == -1)
		{
			paqueteRta->flagProc = 0;
		}
		else{
			paqueteRta->flagProc = reservarProceso(paquete->pid,paquete->nroPag, espacio);
		}
		break;
	}
	case 1: { //caso de Sacar proceso
///retardo?
		printf("Se liberara el proceso %d /n", paquete->pid);
		paqueteRta->flagProc = liberarProceso(paquete->pid);
		break;
	}
	case 2: { //caso de Escritura endisco

		break;
	}
}

	printf("SWAP: El mensaje:  %s\n de la UMC a llegado al Swap.\n",buffer);



//
//	//Me llega un nuevo proceso
//	printf("LLego nuevo proceso al SWAP \n");


	printf("SWAP: me cierro\n");
	return EXIT_SUCCESS;
}

int abrirConfiguracion() {
	int error = 0;
	configuracion = config_create("config_swap.ini");
	printf("Cargando el archivo de configuracion. \n");
	if (configuracion->properties->elements_amount <= 0) {
		perror("No se pudo cargar el archivo de configuración.");
		return -1;
	}

	if(config_has_property(configuracion, "Puerto_Escucha")){
		Puerto_Swap = config_get_int_value(configuracion, "Puerto_Escucha");
		printf("El puerto habilitado es el %d \n", Puerto_Swap);
	}else{
		perror("No se encontro el puerto en el archivo de configuracion");
		error = 1;
	}

	if(config_has_property(configuracion, "Nombre_Swap")){
		Nombre_Swap = config_get_string_value(configuracion, "Nombre_Swap");
		printf("El archivo en disco se llama: %s \n", Nombre_Swap);
	}else{
		perror("No se encontro el nombre del archivo en disco en el archivo de configuracion");
		error = 1;
	}

	if(config_has_property(configuracion, "Cantidad_Paginas")){
		Cantidad_Paginas = config_get_int_value(configuracion, "Cantidad_Paginas");
		printf("El archivo en disco contiene %d paginas\n", Cantidad_Paginas);
	}else{
		perror("No se encontro la cantidad de paginas en el archivo de configuracion");
		error = 1;
	}
	if(config_has_property(configuracion, "Tamanio_Pagina")){
		Tamanio_Pagina = config_get_int_value(configuracion, "Tamanio_Pagina");
		printf("El tamanio de cada pagina es de %d \n", Tamanio_Pagina);
	}else{
			perror("No se encontro el tamanio de paginas en el archivo de configuracion");
			error=1;
	}
		if(config_has_property(configuracion, "Retardo_Compactacion")){
			Retardo_Compactacion = config_get_int_value(configuracion, "Retardo_Compactacion");
			printf("El tiempo de retardo en compactacion, es de: %d \n", Retardo_Compactacion);
		}else{
			perror("No se encontro el retardo de compactacion en el archivo de configuracion");
			error=1;
		}
	if(error){
		return -1;
	}else{
		return 0;
	}
}

int crearArchivo(){
	Nombre_Swap = config_get_string_value(configuracion, "Nombre_Swap");
	printf("El archivo en disco se llama: %s \n", Nombre_Swap);
	FILE *arch = fopen(Nombre_Swap,"w");
	tamanio_archivo = Cantidad_Paginas*Tamanio_Pagina;
	char *vectorArchivo[tamanio_archivo];
	int i;
	char* caracter = "\0";
	for(i=0;i<tamanio_archivo;i++){
		vectorArchivo[i] = caracter;
	}
	for(i=0;i<tamanio_archivo;i++){
		fwrite(vectorArchivo[i],1,1,arch);
	}

	fclose(arch);

	contadorProcesos = 0;

	return 1;
}

int mapearArchivo(){
	int fd;
	size_t length = tamanio_archivo;
	discoSwapMapped=malloc(tamanio_archivo);
	discoParaleloNoVirtual = malloc(tamanio_archivo);
	fd = open(Nombre_Swap, O_RDONLY);
	  if (fd == -1) {
		printf("Error abriendo %s para su lectura", Nombre_Swap);
		exit(EXIT_FAILURE);
	    }
	  discoSwapMapped = mmap(0, length,PROT_READ, MAP_SHARED,fd,0);
	if (discoSwapMapped == MAP_FAILED) {
		close(fd);
		printf("Error mapeando el archivo %s \n", Nombre_Swap);
		exit(EXIT_FAILURE);
	    }
	discoMapped = discoSwapMapped;
	int i;
		for(i=0;i<tamanio_archivo;i++){
			discoParaleloNoVirtual[i] = discoMapped[i];
		}
	printf("Mapeo perfecto  %s \n", Nombre_Swap);
	return 1;
}

int iniciarServidor(int puerto) {

	int sock_lst;  // Escuchar sobre sock_lst, nuevas conexiones sobre new_lst
	struct sockaddr_in my_addr;    // información sobre mi dirección
	my_addr.sin_family = AF_INET;         // Ordenación de bytes de máquina
	my_addr.sin_port = htons(Puerto);     // short, Ordenación de bytes de la red
	my_addr.sin_addr.s_addr = INADDR_ANY; // rellenar con mi dirección IP
	memset(&(my_addr.sin_zero), '\0', 8); // poner a cero el resto de la estructura
	int yes=1;

	if ((sock_lst = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return -1;
	}

	if (setsockopt(sock_lst,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		perror("setsockopt");
		return -1;
	}

	if (bind(sock_lst, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		return -1;
	}

	if (listen(sock_lst, 10) == -1) {
		perror("listen");
		return -1;
	}

	return sock_lst;
}

int establecerConexion(int sock_lst) {
	int new_lst;
	struct sockaddr_in umcAddress; // información sobre la dirección del cliente
	unsigned int sin_size = sizeof(struct sockaddr_in);
	if ((new_lst = accept(sock_lst, (struct sockaddr *)&umcAddress, &sin_size)) == -1)
	{
		perror("accept");
	}
	else {
        printf("SWAP: Nueva conexion desde %s en" "socket %d\n",inet_ntoa(umcAddress.sin_addr), new_lst);
        return -1;
	}
	return new_lst;
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
			if(i == Cantidad_Paginas -1){
				return -1;
			}
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
	for(i=nroPag+1;i<Cantidad_Paginas;i++){
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
	for(i=numeroPag;i<Cantidad_Paginas;i++){
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
	for(i=0;i<Cantidad_Paginas;i++){
		if(paginasSWAP[i].ocupada ==0){
			return primerPagLibre= i;
		}
	}
	return -1;
}

int reservarProceso(int pidProceso, int cantPags , int pagAPartir){
	int error;
	error = asignarEspacio(cantPags, contadorProcesos, pidProceso, pagAPartir);
	if(error == -1){
		printf("Hubo error al asignar el proceso %d /n", pidProceso);
		return 0;
	}
	printf("Se reservo el espacio para el proceso con PID: %d de %d paginas /n", pidProceso, cantPags);
	return 1;
}

int asignarEspacio(int cantPagsAAsignar, int contadorP, int proceso, int inicio){
	int primerPaginaLibre = inicio;
	int i;
	if(inicio +cantPagsAAsignar > Cantidad_Paginas)
	{
		return -1;
	}
	for(i=0;i<cantPagsAAsignar;i++){
		printf("Se asignara la pagina %d al proceso %d /n", primerPaginaLibre+i,proceso);
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
	printf("SE INVOCO AL MODULO DE COMPACTACION... /n");
	printf("INICIANDO COMPACTACION /n");
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
			printf("No hay mas paginas que compactar, finalizando correctamente. /n");
			break;
		}
		leerPagina(primerPaginaOcupada, &inicioOcupada, &finOcupada);
		leerPagina(primerPaginaLibre,&inicioLibre,&finLibre);
		printf("La pagina ocupada %d pasara a la pagina %d libre /n", primerPaginaOcupada, primerPaginaLibre);
		for(i=0;i<Tamanio_Pagina;i++)
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
	printf("Terminando compactacion.../n");
	sleep(Retardo_Compactacion);
	return resultado;
}

int primerPaginaOcupadaLuegoDeUnaLibre(){
	int estadoPaginaSiguiente = -1;
	int estadoPaginaAnterior =-1;
	int i;
	for(i=0;i<Cantidad_Paginas;i++){
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
	*inicio = nroPag*Tamanio_Pagina;
	if(*inicio >tamanio_archivo)
	{
		printf("No se puede leer la pagina /n");
		return -1;
	}
	*fin = *inicio +Tamanio_Pagina-1;
	return 1;
}

int hayPaginasOcupadasLuegoDeLaUltimaLibre(){
	int i;
	int flagExistencia;
	for(i=ultimaPagLibre();i<Cantidad_Paginas;i++){
		if(paginasSWAP[i].ocupada == 1)
		{
			flagExistencia=1;
		}
	}
	return flagExistencia;
}

int updatearArchivoDisco(){
	FILE* archendisco =fopen(Nombre_Swap, "w");;
	int i =0;
	printf("Escribiendo en %s, archivo en disco /n", Nombre_Swap);
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
		printf("Borrando pagina %d ocupada por %d /n", primerPaginaDelProceso, idProc);
		paginasSWAP[primerPaginaDelProceso].ocupada = 0;
		paginasSWAP[primerPaginaDelProceso].idProcesoQueLoOcupa = -1;
		primerPaginaDelProceso++; //aumento en 1
		cantPagsOcupadas--;
	}

	printf("Se libero el proceso %d /n", idProc);
	return 0;
}

int getPrimerPagProc(int idProceso){
	int pag;
	int i;
	for(i=0;i<Cantidad_Paginas;i++){
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


