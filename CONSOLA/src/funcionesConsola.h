/*
 * funcionesConsola.h
 *
 *  Created on: 20/5/2016
 *      Author: utnso
 */
#ifndef FUNCIONESCONSOLA_H_
#define FUNCIONESCONSOLA_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <commons/config.h>
#include "../../COMUNES/handshake.h"
//#include "../../COMUNES/nsockets.h"
#include "../../COMUNES/nsockets.c"
#include <commons/log.h>
#include <commons/collections/list.h>
#include <theDebuggers/socketLibrary.h>

char *ip_nucleo, *puerto_nucleo;

void archivoDeConfiguracion();
char* leerElArchivo(FILE *archivo);
int conectarConElNucleo();
int enviarInformacionAlNucleo(char * script, int nucleo);



#endif /* FUNCIONESCONSOLA_H_ */
