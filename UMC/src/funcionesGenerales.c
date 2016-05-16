/*
 * funcionesGenerales.c
 *
 *  Created on: May 15, 2016
 *      Author: utnso
 */
#include <commons/config.h>
#include <stdlib.h>
#include "funciones.h"

// leer archivo de configuracion

void leerArchivoDeConfiguracion()
{
	t_config* configuracion;
	configuracion = config_create("Ruta al archivo de configuracion");

//	if (config_has_property(configuracion,"IP_SWAP"))
//		IPSWAP = config_get_string_value(configuracion,"IPSWAP");
//	else
//	{
//		perror("No esta configurada la ip del swap");
//		return 1;
//	}

	if (config_has_property(configuracion,"PUERTO_SWAP"))
		PUERTO_SWAP = config_get_int_value(configuracion,PUERTO_SWAP);
	else
	{
		perror("No esta configurado el puerto del swap");
		return;
	}

	if (config_has_property(configuracion,"PUERTO_NUCLEO"))
		PUERTO_NUCLEO = config_get_int_value(configuracion,PUERTO_NUCLEO);
	else
	{
		perror("No esta configurado el puerto del nucleo");
		return;
	}

	if (config_has_property(configuracion,"PUERTO_CPU"))
		PUERTO_CPU = config_get_int_value(configuracion,PUERTO_CPU);
	else
	{
		perror("No esta configurado el puerto del cpu");
		return;
	}

}
