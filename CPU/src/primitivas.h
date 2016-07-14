/*
 * primitivas.h
 *
 *  Created on: 26/4/2016
 *      Author: Franco Albornoz
 */

#ifndef PRIMITIVAS_H_
#define PRIMITIVAS_H_
//#include "../../COMUNES/estructurasControl.h"
#include <theDebuggers/estructurasControl.h>

void armarDireccionPrimeraPagina(t_direccion* direccionReal);
void armarProximaDireccion(t_direccion* direccionReal);
int primeraPagina();
void armarDirecccionDeFuncion(t_direccion* direccionReal);
void proximaDireccion(int posStack, int posUltVar, t_direccion* direccionReal);
void buscarUbicacionVaraible(int posicionStack, t_nombre_variable identificador_variable, t_direccion* direccionReal);
int convertirDireccionAPuntero(t_direccion* direccion);
void convertirPunteroADireccion(int puntero, t_direccion* direccion);
void enviarDirecParaEscribirUMC(char* UMC, t_direccion* direccion, int valor);
void enviarDirecParaLeerUMC(char* UMC, t_direccion* direccion);
t_puntero definirVariable(t_nombre_variable identificador_variable);
t_puntero obtenerPosicionVariable (t_nombre_variable identificador_variable);
t_valor_variable dereferenciar(t_puntero direccion_variable);
void asignar(t_puntero direccion_variable,t_valor_variable valor);
t_valor_variable obtenerValorCompartida(t_nombre_compartida variable);
t_valor_variable asignarValorCompartida(t_nombre_compartida variable,t_valor_variable valor);
void irAlLabel(t_nombre_etiqueta etiqueta);
//t_puntero_instruccion llamarFuncion(t_nombre_etiqueta etiqueta,t_puntero donde_retornar,t_puntero_instruccion linea_en_ejecucion);
void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar);
void retornar(t_valor_variable retorno);
void imprimir(t_valor_variable valor_mostrar);
void imprimirTexto(char*texto);
void entradaSalida(t_nombre_dispositivo dispositivo,int tiempo);
void finalizar();
void wait_kernel(t_nombre_semaforo identificador_semaforo);
void signal_kernel(t_nombre_semaforo identificador_semaforo);

#endif /* PRIMITIVAS_H_ */
