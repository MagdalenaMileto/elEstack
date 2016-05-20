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


int conexionConElNucleo(void);
char* leerArchivo(FILE *archivo);



#endif /* FUNCIONESCONSOLA_H_ */
