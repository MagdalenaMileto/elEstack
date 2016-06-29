/*
 * testSwap.c
 *
 *  Created on: 27/6/2016
 *      Author: utnso
 */

#include <cspecs/cspec.h>
#include "../src/control.h"
#include <commons/collections/list.h>

context(primera) {
	describe("inicializar proceso") {

		it("inicializa la cantidad de paginas correctas") {

			inicializarControlPaginas(5);

			should_int(list_size(paginasSWAP)) be equal to(5);

			bool estanLibres(void * elemento) {

				t_controlPaginas * entrada = (t_controlPaginas *) elemento;
				return entrada->libre;
			}

			should_bool(list_all_satisfy(paginasSWAP,estanLibres)) be equal to(true);

		}end

	}end

	describe("puede inicializar") {

		it("con el espacio de corrido") {

			inicializarControlPaginas(5);

			should_bool(sePuedeInicializar(3)) be equal to(true);
			should_bool(sePuedeInicializar(5)) be equal to(true);
			should_bool(sePuedeInicializar(6)) be equal to(false);

		}end

		it("con el espacio mezclado") {

			inicializarControlPaginas(5);

			void marcar_ocupado(t_controlPaginas * pagina){
				pagina->libre = false;
			}

			marcar_ocupado(list_get(paginasSWAP,1));
			marcar_ocupado(list_get(paginasSWAP,2));
			marcar_ocupado(list_get(paginasSWAP,4));


			should_bool(sePuedeInicializar(1)) be equal to(true);
			should_bool(sePuedeInicializar(2)) be equal to(false);
			should_bool(sePuedeInicializar(4)) be equal to(false);

		}end

	}end

	describe("Inicializar") {

			it("inicializa procesos") {

				inicializarControlPaginas(5);

				int pid = 1;
				int pagina = 3;
				void*codigo = "fansdfand faoisdjf sadsd faoisdjf asdfa asdf dioajdf faoisdjf asdfa asdf dioajdf faoisdjf asdfa asdf dioajdf faoisdjf asdfa asdf dioajdf asodijfa dioajdf";
				inicializarProceso(pid, pagina, codigo);

				int pid2 = 10;
				int pagina2 = 1;
				void*codigo2 = "sadsd faoisdjf asdfa asdf dioajdf";
				inicializarProceso(pid2, pagina2, codigo2);



				int pid3 = 5;
				int pagina3 = 2000;
				void*codigo3 = "asdf faoisdjf asdf asdf dioajdf";

				inicializarProceso(pid3, pagina3, codigo3);



				bool compararPid(t_controlPaginas * pagina, int pidd){
					return pagina->pid == pidd;
				}

				should_bool(compararPid(list_get(paginasSWAP,0), 1)) be equal to (true);
				should_bool(compararPid(list_get(paginasSWAP,1), 1)) be equal to (true);
				should_bool(compararPid(list_get(paginasSWAP,2), 1)) be equal to (true);

				should_bool(compararPid(list_get(paginasSWAP,3), 1)) be equal to (false);

				should_bool(compararPid(list_get(paginasSWAP,3), 10)) be equal to (true);
				should_bool(sePuedeInicializar(pagina3)) be equal to(false);


			//	should_bool(compararPid(list_get(paginasSWAP, 0)) be equal to (true));


			}end

		}end
}
