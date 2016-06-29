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
	describe("inicializar Control SWAP") {

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

	describe("CASE 1: Hay espacio para programas nuevos") {

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

			marcar_ocupado(list_get(paginasSWAP,0));
			//marcar_ocupado(list_get(paginasSWAP,2));
			marcar_ocupado(list_get(paginasSWAP,4));


			should_bool(sePuedeInicializar(1)) be equal to(true);
			should_bool(sePuedeInicializar(2)) be equal to(true);
			should_bool(sePuedeInicializar(3)) be equal to(true);

		}end

	}end

	describe("Inicializar Procesos en Memoria") {

			it("inicializa procesos") {

				abrirConfiguracion();

				crearArchivo();
				mapearArchivo();
				inicializarControlPaginas(12);

				int pid = 1;
				int pagina = 3;
				void*codigo = malloc(256*pagina);
				char cod[709] = "Contadordecarsacteres.com es un contador automático de caracteres y palabras en un texto. Solo colocque el cursor dentro de la caja de textos y comienze a escribir y el contador de caracteres comenzara a contar sus palabras y caracteres a medida de que usted vaya escribiendo También puede copiar y pegar bloques de texto de un documento que tengas ya escrito. Solo pegue el texto dentro de la caja del contador y rápidamente se mostrara la cantidad de caracteres y palabras que contenga el texto pegado. Esta herramienta es útil para sus tweets en Twitter, y también para una multitud de otras aplicaciones. El límite de caracteres en twitter es de 140. Un mensaje de texto a tu celular el límite es ";
				memset(codigo, '\0', 256 * pagina);
				memcpy(codigo, cod, 700);

				//void*codigo = "fansdfand faoisdjf sadsd faoisdajdf asodijfa dioajdf";
				inicializarProceso(pid, pagina, codigo);

				int pid2 = 10;
				int pagina2 = 1;
				void*codigo2 = malloc(256*pagina2);
				char cod2[200] = "160. Anuncios de google son 25 caracteres para el título, 70 para el cuerpo del anuncio y 35 para la url que se muestra. El contador también es de gran ayuda a reporteros, para tareas escolares";
				memset(codigo2, '\0', 256 * pagina2);
				memcpy(codigo2, cod2, 190);


				//void*codigo2 = "sadsd faoisdjf asdfa asdf dioajdf";
				inicializarProceso(pid2, pagina2, codigo2);



//				int pid3 = 5;
//				int pagina3 = 2000;
//				void*codigo3 = "asdf faoisdjf asdf asdf dioajdf";
//
//				inicializarProceso(pid3, pagina3, codigo3);

				int pid4 = 4;
				int pagina4 = 5;
				//void*codigo4 = "asdf faoisdjf asdf asdf dioajdf";
				void*codigo4 = malloc(256*pagina4);
				char cod4[1113] = "Este proceso simulará el administrador de memoria virtual de un sistema operativo. Será el encargado de administrar la memoria virtual del sistema, para ello, al iniciarse, creará un archivo de tamaño configurable (en páginas), el cual representará nuestra partición de swap, y quedará a la espera de la conexión del proceso UMC. Inicialmente el archivo de swap deberá ser rellenado con el caracter \0, a fines de inicializar la partición9. El tamaño de las páginas escritas en swap es configurable10, así como también el nombre de este archivo. Para que el manejo del espacio libre y ocupado en esta partición sea sencillo, se utilizará un esquema de asignación contigua. La partición de swap será considerada inicialmente como un hueco del total de su tamaño, medido en cantidad de páginas que puede alojar. Ante la llegada de un Programa AnSISOP, asignará el tamaño necesario para que este sea guardado, dejando el espacio restante como libre. Esto mismo sucederá con los siguientes Programas AnSISOP puestos en ejecución. Al finalizar un Programa AnSISOP, el espacio que tenía asignado será marcado como libre.";
				memset(codigo4, '\0', 256 * pagina4);
				memcpy(codigo4, cod4, 1114);

				inicializarProceso(pid4, pagina4, codigo4);

				int pid5 = 12;
				int pagina5 = 2;
				void*codigo5 = malloc(256*pagina5);
				char cod5[360] = "“Feliz de poder compartir con todos ustedes otra vez lo que la gente hoy denomina como el campeonato del mundo que más gozó. Y lo hicimos nosotros, muchachos. Por eso cuenten conmigo, tengo un par de trabajos que hacer acá, pero cuenten conmigo. Muéstrenle a todo el mundo que yo soy parte de ustedes, porque los que jugamos en el Azteca fuimos nos";
				memset(codigo5, '\0', 256 * pagina5);
				memcpy(codigo5, cod5, 346);


				//void*codigo2 = "sadsd faoisdjf asdfa asdf dioajdf";
				inicializarProceso(pid5, pagina5, codigo5);


				bool compararPid(t_controlPaginas * pagina, int pidd){
					return pagina->pid == pidd;
				}

				should_bool(compararPid(list_get(paginasSWAP,0), 1)) be equal to (true);
				should_bool(compararPid(list_get(paginasSWAP,1), 1)) be equal to (true);
				should_bool(compararPid(list_get(paginasSWAP,2), 1)) be equal to (true);

				should_bool(compararPid(list_get(paginasSWAP,3), 1)) be equal to (false);

				should_bool(compararPid(list_get(paginasSWAP,3), 10)) be equal to (true);
				//should_bool(sePuedeInicializar(pagina3)) be equal to(false);

//
//				should_bool(compararPid(list_get(paginasSWAP,4), 5)) be equal to (true);
//				should_bool(compararPid(list_get(paginasSWAP,5), 5)) be equal to (true);
//				should_bool(compararPid(list_get(paginasSWAP,6), 5)) be equal to (true);
				//should_bool(compararPid(list_get(paginasSWAP,2005), 4)) be equal to (true);

				should_bool(compararPid(list_get(paginasSWAP,4), 4)) be equal to (true);
				should_bool(compararPid(list_get(paginasSWAP,5), 4)) be equal to (true);
				should_bool(compararPid(list_get(paginasSWAP,6), 4)) be equal to (true);


			//	should_bool(compararPid(list_get(paginasSWAP, 0)) be equal to (true));


			}end

		}end

		describe("Probar Compactacion") {

					it("Sacar procesos") {
						finalizar_proceso(10);
						finalizar_proceso(12);

						bool estanLibresPags(void * elemento) {

							t_controlPaginas * entrada = (t_controlPaginas *) elemento;
							return entrada->libre;
						}

						should_bool(estanLibresPags(list_get(paginasSWAP,3))) be equal to (true);
						should_bool(estanLibresPags(list_get(paginasSWAP,9))) be equal to (true);
						should_bool(estanLibresPags(list_get(paginasSWAP,10))) be equal to (true);

					}end

					it("Meter proceso y Compactar") {

						int pid6 = 33;
						int pagina6 = 4;
						void*codigo6 = malloc(256*pagina6);
						char cod6[960] = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";
						memset(codigo6, '\0', 256 * pagina6);
						memcpy(codigo6, cod6, 939);

						compactacion();



						should_bool(sePuedeInicializar(4)) be equal to(true);

						inicializarProceso(pid6, pagina6, codigo6);
//						bool compararPid2(t_controlPaginas * pagina, int pidd){
//										return pagina->pid == pidd;
//									}
//
//
//
//
//
//						//should_bool(compararPid2(list_get(paginasSWAP,8), 33)) be equal to (true);
//						should_bool(compararPid2(list_get(paginasSWAP,9), 33)) be equal to (true);
//						should_bool(compararPid2(list_get(paginasSWAP,10), 33)) be equal to (true);
//						//should_bool(compararPid2(list_get(paginasSWAP,11), 33)) be equal to (true);
//


					}end

				}end

}
