#include "umc.h"

int main(int argc, char** argv) {

	log2 = log_create("NICO", "UMC", 0, LOG_LEVEL_INFO);
	log = log_create(ARCHIVOLOG, "UMC", 0, LOG_LEVEL_INFO);
	log_info(log, "Iniciando UMC.\n");

	setbuf(stdout, NULL);

	inicializar_semaforos();
	levantar_configuraciones();
	inicializar_marcos();
	solicitar_bloque_memoria();
	comunicarse_con_el_swap();
	esperar_al_nucleo();
	atender_conexiones();
	atender_hilo_consola();
	close(socket_conexiones_nuevas);
	close(socket_swap);
	return EXIT_SUCCESS;
}

void inicializar_semaforos() {
	pthread_mutex_init(&semaforo_mutex_cpu, NULL);
	log_info(log, "Se inicializan los semaforos.\n");
}

void levantar_configuraciones() {

	t_config * archivo_configuracion = config_create("UMC.confg");

	puerto_umc = config_get_string_value(archivo_configuracion, "PUERTO");
	ip_swap = config_get_string_value(archivo_configuracion, "IP_SWAP");
	puerto_swap = config_get_string_value(archivo_configuracion, "PUERTO_SWAP");
	cantidad_marcos = config_get_int_value(archivo_configuracion, "MARCOS");
	tamanio_marco = config_get_int_value(archivo_configuracion, "MARCOS_SIZE");
	cantidad_maxima_marcos = config_get_int_value(archivo_configuracion,
			"MARCO_X_PROD");
	entradas_TLB = config_get_int_value(archivo_configuracion, "ENTRADAS_TLB");
	retardo = config_get_int_value(archivo_configuracion, "RETARDO");
	algoritmo = config_get_int_value(archivo_configuracion, "ALGORITMO");

	log_info(log, "Se levantan con exito las configuraciones.\n");

}

void comunicarse_con_el_swap() {

	socket_swap = conectar_a(ip_swap, puerto_swap);
	/*
	 bool resultado = realizar_handshake(socket_swap);

	 if (!resultado) {
	 error_show("No se autenticó la conexión con el swap");
	 exit(EXIT_FAILURE);
	 }*/

	log_info(log, "Conexion con SWAP.\n");

}

void esperar_al_nucleo() {

	socket_conexiones_nuevas = socket_escucha("localhost", puerto_umc);

	listen(socket_conexiones_nuevas, 1024);

	socket_nucleo = aceptar_conexion(socket_conexiones_nuevas);
//	bool resultado = esperar_handshake(socket_nucleo);
	//if (resultado) {
	pthread_create(&hilo_nucleo, NULL, (void *) atender_nucleo, NULL);
	//} else {
	//error_show("No se autenticó la conexión con el Nucleo");
	//	exit(EXIT_FAILURE);
//	}

	log_info(log, "Conexion con Nucleo.\n");
}

void atender_conexiones() {

	int * nuevo_socket_cpu;
	pthread_t * nuevo_hilo_cpu;

	while (1) {

		nuevo_socket_cpu = (int *) malloc(sizeof(int));

		*nuevo_socket_cpu = aceptar_conexion(socket_conexiones_nuevas);

		bool resultado = esperar_handshake(*nuevo_socket_cpu);

		if (resultado) {

			nuevo_hilo_cpu = (pthread_t *) malloc(sizeof(pthread_t));
			pthread_create(nuevo_hilo_cpu, NULL, (void *) atender_cpu,
					(void *) nuevo_socket_cpu);
		} else {

			printf("Conexión no autenticada en el socket %d\n",
					*nuevo_socket_cpu);
		}

	}
}

void solicitar_bloque_memoria() {

	memoria = malloc(tamanio_marco * cantidad_marcos);

	if (memoria == NULL) {
		error_show("No se pudo otorgar la memoria solicitada");
		exit(EXIT_FAILURE);
	}

}

void atender_hilo_consola() {

	pthread_t nuevo_hilo_consola;

	pthread_create(&nuevo_hilo_consola, NULL, esperar_comando, NULL);
}

