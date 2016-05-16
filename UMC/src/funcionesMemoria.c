/*
 * Funciones.c
 *
 *  Created on: 14 de may. de 2016
 *      Author: Micaela Paredes
 */

#include "umc.c"

	void inicializarDireccionesLogicas(void)
	{
		int i = 0;
		for(;i <= (MARCOS - 1); i++)
		{
			direccionesLogicas[i] = 0;
		}
	}

	void inicializarTablaDePaginas(void)
	{
		int i = 0;
		for(;i<=MARCO_SIZE; i++)
		{
			tablaDePaginas[i] = 0;
		}
	}

	void inicializarPrograma(int idProg, int paginasRequeridas)
	{
		int c = 0;
		while(direccionesLogicas[c] == 0)
		{
			c++;
		}
		int limite = c + paginasRequeridas;
		for(; c<= limite; c++)
		{
			direccionesLogicas[c] = idProg;
		}
			// void informarInicializacionASwap(int paginasRequeridas, processid){}

	}


		/*	void almacenarBytes(int pagina, int offset, int tamanioBuffer, int buffer[tamanioBuffer]){

	}

	void solicitarBytes(int pagina, int offset, int cantidadBytes){

		}

	void cambioDeProcesoActivo(int idProg){
	}

	void finalizarPrograma(int idProg){

		// falta terminar
		// void informarFinalizacionASwap(int idProg){}

		 sleep(1);
	}

		 */

