/*
 * Funciones.c
 *
 *  Created on: 14 de may. de 2016
 *      Author: Micaela Paredes
 */

	void inicializarDireccionesLogicas(void)
	{
		int i = 0;
		for(;i <= (cantidadPaginas - 1); i++)
		{
			direccionesLogicas[i].idPrograma = 0;
			int c = 0;
			for(;c <= (tamanioPagina - 1); c++)
			{
				direccionesLogicas[i].pagina.offset[c] = c;
			}
		}
	}

	void inicializarTablaDePaginas(void)
	{
		int i = 0;
		for(;i<=cantidadPaginas; i++)
		{
			tablaDePaginas[i] = 0;
		}
	}

	void inicializarPrograma(int idProg, int paginasRequeridas)
	{
		int c = 0;
		while(direccionesLogicas[c].idPrograma == 0)
		{
			c++;
		}
		int limite = c + paginasRequeridas;
		for(; c<= limite; c++)
		{
			direccionesLogicas[c].idPrograma = idProg;
		}
			// void informarInicializacionASwap(int paginasRequeridas, processid){}
		sleep(1);
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

