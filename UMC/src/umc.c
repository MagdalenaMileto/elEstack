#include "umc.h"

int main(int argc, char** argv) {

	setbuf(stdout, NULL);

	arrancar_umc();

	borrar_archivos_existentes();
	crear_log();

	inicializar_semaforos();
	levantar_configuraciones();
	inicializar_marcos();
	mostrar_informacion_interna();
	solicitar_bloque_memoria();
	registrar_senial_cierre();
	comunicarse_con_el_swap();
	esperar_al_nucleo();
	atender_hilo_consola();
	atender_conexiones();
	close(socket_conexiones_nuevas);
	close(socket_swap);
	return EXIT_SUCCESS;
}

void inicializar_semaforos() {

	pthread_mutex_init(&semaforo_mutex_tabla_de_paginas, NULL);
	pthread_mutex_init(&semaforo_mutex_tlb, NULL);
	pthread_mutex_init(&semaforo_mutex_swap, NULL);
	pthread_mutex_init(&semaforo_mutex_marcos, NULL);
	pthread_mutex_init(&semaforo_mutex_stats_tlb, NULL);
	pthread_mutex_init(&semaforo_mutex_stats_swap, NULL);
	log_info(log, "\x1b[32mSe inicializan con éxito los semaforos.\n\x1b[0m");
}

void levantar_configuraciones() {

	t_config * archivo_configuracion = config_create("UMC.config");

	puerto_umc = config_get_string_value(archivo_configuracion, "PUERTO");
	ip_swap = config_get_string_value(archivo_configuracion, "IP_SWAP");
	puerto_swap = config_get_string_value(archivo_configuracion, "PUERTO_SWAP");
	cantidad_marcos = config_get_int_value(archivo_configuracion, "MARCOS");
	tamanio_marco = config_get_int_value(archivo_configuracion, "MARCOS_SIZE");
	cantidad_maxima_marcos = config_get_int_value(archivo_configuracion,
			"MARCO_X_PROD");
	entradas_TLB = config_get_int_value(archivo_configuracion, "ENTRADAS_TLB");
	retardo = config_get_int_value(archivo_configuracion, "RETARDO");
	algoritmo = config_get_string_value(archivo_configuracion, "ALGORITMO");

	intervalo_info = config_get_int_value(archivo_configuracion,
			"INTERVALO_INFORMACION");

	log_info(log, "\x1b[32mSe levantan con exito las configuraciones.\n\x1b[0m");

}

void comunicarse_con_el_swap() {

	socket_swap = conectar_a(ip_swap, puerto_swap);

	escrituras_swap = list_create();
	lecturas_swap = list_create();

	log_info(log, "\x1b[32mConexion con SWAP exitosa.\n\x1b[0m");

}

void esperar_al_nucleo() {

	socket_conexiones_nuevas = socket_escucha("localhost", puerto_umc);

	listen(socket_conexiones_nuevas, 1024);

	socket_nucleo = aceptar_conexion(socket_conexiones_nuevas);

	pthread_create(&hilo_nucleo, NULL, (void *) atender_nucleo, NULL);

	enviar(socket_nucleo, 13, sizeof(int), &tamanio_marco);

	log_info(log, "\x1b[32mConexion con Nucleo exitosa.\n\x1b[0m");
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

			printf("\x1b[32mConexión no autenticada en el socket %d.\n\x1b[0m",
					*nuevo_socket_cpu);
		}

	}
}

void solicitar_bloque_memoria() {

	memoria = malloc(tamanio_marco * cantidad_marcos);

	tabla_de_paginas = list_create();

	tlb = list_create();

	aciertos_tlb = list_create();

	fallos_tlb = list_create();

	if (memoria == NULL) {
		error_show("\x1b[31mNo se pudo otorgar la memoria solicitada.\n\x1b[0m");
		exit(EXIT_FAILURE);
	}

}

void atender_hilo_consola() {

	pthread_t nuevo_hilo_consola;

	pthread_create(&nuevo_hilo_consola, NULL, esperar_comando, NULL);
}

void mostrar_informacion_interna() {
	pthread_t hilo_mostrar_en_pantalla;

	pthread_create(&hilo_mostrar_en_pantalla, NULL, mostrar_estado_interno,
	NULL);
}

void borrar_archivos_existentes() {

	remove("UMC.log");
	remove("memory.dump");
}

void crear_log() {

	log = log_create(ARCHIVOLOG, "UMC", 0, LOG_LEVEL_INFO);
	log_info(log, "Iniciando UMC.\n");

}

void registrar_senial_cierre() {

	log_info(log, "\x1b[31mSe registra la senial de interrupcion.\n\x1b[0m");

	void cerrar_umc(int senal) {

		log_info(log, "\x1b[31mSe cierra la UMC.\n\x1b[0m");

		exit(EXIT_FAILURE);

	}

	signal(SIGINT, cerrar_umc);
}

void arrancar_umc() {

	printf("%s", "\n\n====== INICIO ======\n\n");
}
