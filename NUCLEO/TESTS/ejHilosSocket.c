
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>



#include "../../COMUNES/nsockets.c"


#include <signal.h>

int cliente1,cliente2;
void intHandler(int dummy);






 struct arg_struct {
  int socket;

};


void *hilo_Handler_Conexiones(void *arg);
void *hilo_Conexion(void *arg);





int main(){

	signal(SIGINT, intHandler);
	char mensaje[100]="El Barto";


  	pthread_t thHandler_Conexiones;
  	pthread_create(&thHandler_Conexiones, NULL, hilo_Handler_Conexiones, NULL);


  	sleep(2); //Espero a que los hilos levanten


  	//Creo un cliente y lo conecto

	cliente1 = cliente("127.0.0.1",5001);

	if(cliente1==0){
		printf("No encontre conexion me cierro :'( \n");
	  	exit (EXIT_FAILURE);
	}
	
	
	sleep(1);


	//Creo un cliente y lo conecto

	cliente2 = cliente("127.0.0.1",5001);

	if(cliente2==0){
		printf("No encontre conexion me cierro :'( \n");
	  	exit (EXIT_FAILURE);
	}
	

	
	t_header header;

	header.id = 101;
	header.size = strlen(mensaje);
	header.data = mensaje;
		
		
	sleep(1);
	
	printf("Envie %s\n",mensaje);
	enviar_paquete(cliente1, header);
	close(cliente1);

	sleep(1);

	printf("Envie %s\n",mensaje);

	enviar_paquete(cliente2, header);
	close(cliente2);






	pthread_join(thHandler_Conexiones, NULL);	//Me bloqueo esperando a que terminen los hilos (que en este caso no termina nunca el hilo handler conexiones)

}






void *hilo_Conexion(void *arg){

    struct arg_struct *args = (struct arg_struct *)arg;
    int estado;


    while(1){

        t_header estructuraARecibir;


          estado=recibir_paquete(args->socket,&estructuraARecibir);


          if(estado==-1){  //Evento desconexion

               printf("Cerró Conexion %d\n",args->socket);
               //Deberia hacer todos los free del hilo
               break;
        
          }

          
          if(estructuraARecibir.id==101){									//Puedo enviar varios paquetes, PCB, handshake, etc. con esto identifico cual es
            	printf("Recibi %s\n",(char*)estructuraARecibir.data);  
          }






          //Lo que recibi lo elimino
          estructuraARecibir.id=0; //Por las dudas, creo que no haria falta
          free(estructuraARecibir.data);
          
     }

     return 0;

}



void *hilo_Handler_Conexiones(void *arg){

     int servidorSocket,socketCliente;
     struct sockaddr_in addr;     
     socklen_t addrlen = sizeof(addr);
     struct arg_struct *args; 


     servidorSocket=servidor(5001);


     if(servidorSocket==-1){
          printf("El AMEO NO ME DEJA CREAR SOCKET RE GATO\n");
          close(servidorSocket);
          exit(1);
     }
     
     listen(servidorSocket,5);   

     while(1){


          socketCliente = accept(servidorSocket, (struct sockaddr *) &addr, &addrlen);

         
          args = malloc(sizeof(struct arg_struct));//cuando se termine el proceso hacer un free de esto
          args->socket=socketCliente;

          pthread_t thCONEXION;
          pthread_create(&thCONEXION, NULL, hilo_Conexion, (void *)args);

          printf("Acepté cliente: %d\n",socketCliente);
     }

}






void intHandler(int dummy) {

	//Aca deberia cerrar todos los socket y hacer un free de todas las estructuras

  printf("cierro Todo...\n\n");
  exit(0);
}


