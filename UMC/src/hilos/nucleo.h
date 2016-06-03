/*
 * nucleo.h
 *
 *  Created on: 29/4/2016
 *      Author: utnso
 */

#ifndef SRC_HILOS_NUCLEO_H_
#define SRC_HILOS_NUCLEO_H_

#include "../pedidos/inicializar.h"
#include "../pedidos/finalizar.h"
#include "../globales.h"

#include <stdio.h>

#include <theDebuggers/socketLibrary.h>


#define EXITO 1
#define FRACASO -1

#define INICIALIZAR 4
#define FINALIZAR 6

void atender_nucleo();

#endif /* SRC_NUCLEO_H_ */
