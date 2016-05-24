/*
 * nucleo.c
 *
 *  Created on: 23/4/2016
 *      Author: nico
 */



#include "nucleo.h"


#define ERROR -1

/* VARIABLES GLOBALES */
int * CONSTANTE;

typedef struct {
  int puerto_programas;
  int puerto_cpus;
  char* ip_umv;
  int puerto_umv;
  int quantum;
  int sizePagina;

}CONF_KERNEL;

CONF_KERNEL *config_kernel;



//Colas PLP
t_queue* cola_new;
t_queue* cola_exit;

//Colas PCP
t_queue* cola_exec;
t_queue* cola_ready;
t_queue* cola_block;

pthread_mutex_t mutex_cola_new;

int pidcounter=0;


// Listas que maneja el PCP
t_queue* cola_CPU_libres;


 struct arg_struct {
  int socket;

};





//Eliminar todo lo que sea de mas de aca
int listenningSocket,socketCliente,servidorSocket,servidorCPU,clienteSocket,losClientes,clientesCPU,umc,ultimoCPU;

char mensaje[92]="¡Me gusta esa teoría! Tal vez son quintillizos.... todos se llaman Petyr, todos están ob";


/* FIN DE VARIABLES GLOBALES */





int main(){
    
      signal(SIGINT, intHandler);

      printf("NUCLEO: INICIÓ\n");


      config_kernel = malloc(sizeof(CONF_KERNEL));



      config_kernel->sizePagina=50;



//MOCKS


      pthread_t mock;
      pthread_create(&mock, NULL, hilo_mock, NULL);

      sleep(3);


     
      //Inicializaciones -> lo podriamos meter en una funcion externa

      cola_new =queue_create();
      cola_exec =queue_create();
      cola_ready = queue_create();
      cola_block = queue_create();
      cola_exit = queue_create();
      cola_CPU_libres = queue_create();
 
      pthread_mutex_init(&mutex_cola_new,NULL);

     

      pthread_t thPCP, thPLP,thCONEXIONES_CPU,thCONEXIONES_CONSOLA;

      pthread_create(&thCONEXIONES_CONSOLA, NULL, hilo_HANDLER_CONEXIONES_CONSOLA, NULL);
      pthread_create(&thCONEXIONES_CPU, NULL, hilo_HANDLER_CONEXIONES_CPU, NULL);
  

      pthread_create(&thPCP, NULL, hilo_PCP, NULL);
      pthread_create(&thPLP, NULL, hilo_PLP, NULL);


       conectarUmc();

      pthread_join(thCONEXIONES_CONSOLA, NULL);
      pthread_join(thCONEXIONES_CPU, NULL);

      pthread_join(thPCP, NULL);
        pthread_join(thPLP, NULL);




	//Terminamos
}



void conectarUmc(void){

  //Handlear errores; El programa debe salir si no conecta umc? remite conexion? que hacemo?

  umc = cliente("127.0.0.1",1207);
  if(umc==0){
    printf("NUCLEO: No encontre UMC me cierro :'( \n");
      exit (EXIT_FAILURE);
 }
 printf("Nucleo: Conecté bien umc %d\n",umc);



  
        //Handlear errores  ****** ****** ****** ****** ****** ****** ****** ****** ****** ****** ******
    

        int estado;
        estado=handshake(umc, 202,201);
       
        if(estado==1){
            printf("NUCLEO:Handshake exitoso con umc\n");  
        }else{
            printf("NUCLEO:Handshake invalido con umc\n");
        }





}

t_proceso* dameProceso(t_queue *cola, int sock ){
  int a=0,t;
  t_proceso *w;
  while(w=(t_proceso*)list_get(cola->elements,a)){
   
    if(w->socket_CPU==sock) return (t_proceso*)list_remove(cola->elements,a);
    a++;
  }
  return NULL;
}





void *hilo_PLP(void *arg){
t_proceso *proceso;

//Hay un proceso en new mando a ready 
//hacerlo con semafoto para que bloquee

    while(1){
   		if(queue_size(cola_new)!=0){			
            proceso=queue_pop(cola_new);
   			printf("Nucleo: Saco proceso  %d new, mando a ready\n",proceso->pcb.pid);   		
   			queue_push(cola_ready,proceso);
   		}	
    }

}




void mandarAEjecutar(t_proceso *proceso,int sock){
  t_header header; int estado;
  header.id=303;
  header.data=&proceso->pcb;
  header.size=sizeof(proceso->pcb);
  proceso->socket_CPU=sock;
  enviar_paquete((int)sock,header);
}


int handlerErrorCPU(int error,int sock){
    t_proceso *proceso;
    if(error == -1){
        if((proceso=dameProceso(cola_exec,sock))||(proceso=dameProceso(cola_ready,sock))||(proceso=dameProceso(cola_block,sock))||(proceso=dameProceso(cola_new,sock))||(proceso=dameProceso(cola_exit,sock))){
           enviar_id(proceso->socket_CONSOLA,108);
           return -1;
        }
    }
    return 0;
}


void *hilo_PCP(void *arg){

t_proceso *proceso;int sock;
    while(1){

    	if(queue_size(cola_ready)!=0&&queue_size(cola_CPU_libres)!=0){
   		

        sock=(int)queue_pop(cola_CPU_libres);
   			proceso=queue_pop(cola_ready);
        printf("Nucleo: Saco proceso %d ready, mando a exec\n",proceso->pcb.pid);

   			queue_push(cola_exec,proceso);
        mandarAEjecutar(proceso,sock);
   		}	



    //Do something yoooo
    }

}


t_proceso* crearPrograma(int sock){


      t_proceso* procesoNuevo;
      procesoNuevo=malloc(sizeof(t_proceso));
      procesoNuevo->pcb.pid=pidcounter;
      procesoNuevo->socket_CONSOLA=sock;
      pidcounter++;
      return procesoNuevo;

}


void mandarCodigoAUmc(char* codigo,int size){

  int cuantasPaginas, estado,i;
  t_header header;

  cuantasPaginas = ceil((double)size / (double)config_kernel->sizePagina);


  //printf("cuantas paginas%d size%d\n",cuantasPaginas,size);
  
  //Codigo de AMEO TENE X CANTIDAD DE PAGINAS?

    header.id = 204;
    header.size = sizeof(int);


	header.data = &cuantasPaginas;


	printf("***%d*\n",*((int*)header.data));

   estado=enviar_paquete(umc, header);
    //Verificar error envio (desconexion)


    estado=recibir_paquete(umc,&header);



        //Verificar error recepcion(desconexion)

    if(header.id==205){
      if(*((int*)header.data)==1){
        //Hay espacio, mando paginas
      	
        for(i=0;i<cuantasPaginas;i++){


           header.id = 206;
           header.size = config_kernel->sizePagina;
           header.data = codigo+i*(config_kernel->sizePagina);

           estado=enviar_paquete(umc, header);



        }

      }else{
        //No hay espacio, retorno error

      }

    }else{
      //no me deberia haber mandado esto

    }








//(char*)codigo


}



/********************************************************************************
*********************************************************************************
*********************************************************************************
************************CONSOLA STUFF********************************************
*********************************************************************************
*********************************************************************************
*********************************************************************************
*/

void *hilo_CONEXION_CONSOLA(void *arg){

    struct arg_struct *args = (struct arg_struct *)arg;
    int estado;
    t_proceso* proceso;

    if(handshake(args->socket,101,102)!=1){
        printf("NUCLEO:Handshake invalido consola %d\n",args->socket);
        return;
    }
    printf("NUCLEO:Handshake valido consola, creando proceso %d\n",args->socket);

    proceso = crearPrograma(args->socket);

      while(1){
            t_header estructuraARecibir; // Esto tiene que ser un puntero cambiar.....

            estado=recibir_paquete(args->socket,&estructuraARecibir);

            if(estado==-1){
                  printf("Nucleo: Cerro Socket consola\n");
                  break;
                  //Aca deberia eliminar el programa pcb cerrar socket blablabla si ejecuta cpu decile ya fue man UMC
            }

            switch(estructuraARecibir.id){
              case 103:
             
                mandarCodigoAUmc(estructuraARecibir.data,estructuraARecibir.size);
                pthread_mutex_lock(&mutex_cola_new);
                queue_push(cola_new, proceso);
                pthread_mutex_unlock(&mutex_cola_new);
              break;

            }

          
            estructuraARecibir.id=0;
            free(estructuraARecibir.data);
          
     }

}



void *hilo_HANDLER_CONEXIONES_CONSOLA(void *arg){

      int servidorSocket,socketCliente;
      struct sockaddr_in addr;      // Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
      socklen_t addrlen = sizeof(addr);
      struct arg_struct *args; //Probar de sacar afuera esto?

      servidorSocket=servidor(1209);


     if(servidorSocket==-1){
          printf("NUCLEO: No puedo crear socket escucha 1200, me cierro\n");//cambiar a p error
          close(servidorSocket);
          exit(1);
     }

     listen(servidorSocket,5);    //Aca maximas conexiones, ver de cambiar?

     while(1){
          socketCliente = accept(servidorSocket, (struct sockaddr *) &addr, &addrlen);
          
          args = malloc(sizeof(struct arg_struct));//cuando se termine el proceso hacer un free de esto
          args->socket=socketCliente;

          pthread_t thCONEXION_CONSOLA;
          pthread_create(&thCONEXION_CONSOLA, NULL, hilo_CONEXION_CONSOLA, (void *)args);

          printf("NUCLEO: Acepte consola %d\n",socketCliente);
     }

}


/********************************************************************************
*********************************************************************************
*********************************************************************************
************************CPU STUFF************************************************
*********************************************************************************
*********************************************************************************
*********************************************************************************
*/

void *hilo_CONEXION_CPU(void *arg){

    struct arg_struct *args = (struct arg_struct *)arg;
    int estado;
    t_proceso* proceso;
    queue_push(cola_CPU_libres,(void*)args->socket);
         t_header estructuraARecibir;
  while(1){

        
          estado=recibir_paquete(args->socket,&estructuraARecibir);


          if(estado==-1){
               printf("Nucleo: Cerro Socket cpu\n");
               break;
               //Aca deberia eliminar el programa pcb cerrar socket blablabla si ejecuta cpu decile ya fue man UMC
          }

            switch(estado){
              case 304:            
                    proceso=dameProceso(cola_exec,args->socket);
                    printf("NUCLEO: Recibi proceso %d por fin de quantum, encolando en cola ready\n",proceso->pcb.pid);
                    queue_push(cola_ready,proceso);
                    queue_push(cola_CPU_libres,(void *)args->socket);
              break;

                case 320:
                    proceso=dameProceso(cola_exec,args->socket);
                    printf("NUCLEO: Recibi proceso %d por fin de ejecucion, encolando en cola exit\n",proceso->pcb.pid);
                    queue_push(cola_exit,proceso);
                    queue_push(cola_CPU_libres,(void *)args->socket);
                break;

            }


          estructuraARecibir.id=0;
          free(estructuraARecibir.data);
          
     }

}




void *hilo_HANDLER_CONEXIONES_CPU(void *arg){

     struct arg_struct *args; //Probar de sacar afuera esto?
     int servidorSocket,socketCliente;
     struct sockaddr_in addr;      // Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
     socklen_t addrlen = sizeof(addr);


     servidorSocket=servidor(1202);


     if(servidorSocket==-1){
          printf("NUCLEO: No puedo crear socket escucha 1201, me cierro\n");//cambiar a p error
          close(servidorSocket);
          exit(1);
     }
     
     listen(servidorSocket,5);  //Aca maximas conexiones, ver de cambiar?

     while(1){

          socketCliente = accept(servidorSocket, (struct sockaddr *) &addr, &addrlen);

          
          args = malloc(sizeof(struct arg_struct));//cuando se termine el proceso hacer un free de esto
          args->socket=socketCliente;

          pthread_t thCONEXION_CPU;
          pthread_create(&thCONEXION_CPU, NULL, hilo_CONEXION_CPU, (void *)args);

          printf("NUCLEO: Acepte nueva CPU %d\n",socketCliente);
     }

}



void intHandler(int dummy) {
	//clrscr();

	//close(clienteSocket);
    close(servidorSocket);
    close(servidorCPU);
    close(umc);

    printf("NUCLEO: CERRÓ\n");
    printf("cierro Todo...\n\n");
    exit(0);
}



/********************************************************************************
*********************************************************************************
*********************************************************************************
************************MOCKS    ************************************************
*********************************************************************************
*********************************************************************************
*********************************************************************************
*/






int umcMock,clienteUmc;

void *hilo_mock(void *arg){


      struct sockaddr_in addr;  socklen_t addrlen = sizeof(addr);

      pthread_t thmock_consola,thmock_consola2, thmock_cpu;



   pthread_create(&thmock_consola, NULL, hilo_mock_consola, NULL);
     pthread_create(&thmock_consola2, NULL, hilo_mock_consola, NULL);
      pthread_create(&thmock_cpu, NULL, hilo_mock_cpu, NULL);
  


      umcMock=servidor(1207);
      listen(umcMock,5);   
      clienteUmc = accept(umcMock, (struct sockaddr *) &addr, &addrlen);



      printf("UMCMOCK: Acepte conexion%d\n",clienteUmc);

       

        int estado;
        estado=handshake(clienteUmc, 201,202);
       
        if(estado==1){
            printf("UMCMOCK:Handshake exitoso con nucleo\n");  
        }else{
            printf("UMCMOCK:Handshake invalido con nucleo\n");
        }






        t_header header,headerEnviar;

      while(1){

      	estado=recibir_paquete(clienteUmc,&header);

      	if(header.id==204){
      		//printf("UMCMOCK: me pidió %d paginas. Le digo q si\n",*((int*)header.data));
      	
      		headerEnviar.id=205;int a=1;
      		headerEnviar.data=&a;
      		headerEnviar.size=sizeof(int);
      		enviar_paquete(clienteUmc,headerEnviar);

      	}

      	 if(header.id==206){
      	//	printf("UMCMOCK: recibo pagina: %s\n",(char*)header.data);
      	

      	}
      //	printf("Pase\n");
      	free(header.data);


      }


     

}



void *hilo_mock_consola(void *arg){
      int consola;

      sleep(4);

      consola = cliente("127.0.0.1",1209);
      printf("CONSOLAMOCK: Conecté%d\n",consola);


    if(handshake(consola,102,101)!=1){
        printf("CONSOLAMOCK:Handshake invalido nucleo %d\n",consola);
        return;
    }
     printf("CONSOLAMOCK:Handshake valido nucleo %d\n",consola);

     t_header header;
     header.size=sizeof(mensaje);
     header.id=103;
     header.data=mensaje;
    
     enviar_paquete(consola,header);

while(1){}
         // sleep(5);
         
close(consola);
   

}


void *hilo_mock_cpu(void *arg){
      int cpu,estado;


      sleep(5);
      cpu = cliente("127.0.0.1",1202);
      printf("CPUMOCK: Conecté%d\n",cpu);


      
      t_header header,headerEnviar;


          
      while(1){
          estado=recibir_paquete(cpu,&header);  
          if(estado==303){
            printf("CPUMOCK: Recibi pcb... ejecutando\n");
            sleep(2);
            headerEnviar.id=304;
            headerEnviar.data=header.data;
            headerEnviar.size=header.size;
            enviar_paquete(cpu,headerEnviar);

          }


      }

}

