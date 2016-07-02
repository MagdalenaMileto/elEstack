#include "estructurasControl.h"

void destruirPCB(t_pcb *pcb) {
	t_contexto *contexto_a_finalizar;
	while(pcb->sizeContextoActual != 0){
		contexto_a_finalizar= list_get(pcb->contextoActual, pcb->sizeContextoActual-1);
		while(contexto_a_finalizar->sizeVars != 0){
				free((t_direccion *)((t_variable *)list_get(contexto_a_finalizar->vars, contexto_a_finalizar->sizeVars-1))->direccion);
				contexto_a_finalizar->sizeVars--;
			}
		while(contexto_a_finalizar->sizeArgs != 0){
					free((t_direccion*)list_get(contexto_a_finalizar->args, contexto_a_finalizar->sizeArgs-1));
					contexto_a_finalizar->sizeArgs--;
					}
		free(list_get(pcb->contextoActual, pcb->sizeContextoActual-1));
		printf("libere algo\n");
		pcb->sizeContextoActual--;
	}
	printf("estoy por eliminar la lista\n");
	//list_destroy(pcb->contextoActual);
	printf("elimine lista\n");
	free(pcb->indiceDeCodigo);
	printf("free indice de codigo\n");
	free(pcb->indiceDeEtiquetas);
	printf("free etiqute\n");
	free(pcb);
	printf("funco\n");

}

t_pcb *desserializarPCB(char *serializado) {
	char  * serini = serializado;
	int temp;
	int size, i, y;
	t_pcb *pcb;

	pcb = malloc(sizeof(t_pcb));
	memcpy(pcb, serializado, sizeof(t_pcb));
	serializado += sizeof(t_pcb);

	pcb->indiceDeCodigo = malloc(pcb->sizeIndiceDeCodigo * 2 * sizeof(int));
	memcpy(pcb->indiceDeCodigo, serializado, pcb->sizeIndiceDeCodigo * 2 * sizeof(int));
	serializado += pcb->sizeIndiceDeCodigo * 2 * sizeof(int);

	pcb->indiceDeEtiquetas = malloc(pcb->sizeIndiceDeEtiquetas * sizeof(char));
	memcpy(pcb->indiceDeEtiquetas, serializado, pcb->sizeIndiceDeEtiquetas * sizeof(char));

	serializado += pcb->sizeIndiceDeEtiquetas * sizeof(char);


	pcb->contextoActual = list_create();
	


	//serializado += pcb->sizeContextoActual * sizeof(t_contexto*);

//	printf("SIZE CONTEXTO %d\n",pcb->sizeIndiceDeEtiquetas);
//	printf("SIZE CONTEXTO %d\n",pcb->paginasDeCodigo);
//printf("SIZE CONTEXTO %d\n",pcb->sizeContextoActual);
	for (i = 0; i < pcb->sizeContextoActual; i++) {
	

		t_contexto *temp = malloc(sizeof(t_contexto));

		
		memcpy(temp, serializado, sizeof(t_contexto));


		serializado += sizeof(t_contexto);
		temp->vars= list_create();
		temp->args= list_create();

		//printf("SIZE arg %d\n",temp->sizeArgs);
		for (y = 0; y < temp->sizeArgs; y++) {
			t_direccion *dir = malloc(sizeof(t_direccion));
			memcpy(dir, serializado, sizeof(t_direccion));
			serializado += sizeof(t_direccion);
			list_add(temp->args, (void*)dir);
		}
	//	printf("SIZE var %d\n",temp->sizeVars);
		for (y = 0; y < temp->sizeVars; y++) {
		//	printf("AkkkkkSD\n");
			t_variable *var = malloc(sizeof(t_variable));
			//printf("1AkkkkkSD\n");
			var->direccion = malloc(sizeof(t_direccion));
			//printf("2AkkkkkSD\n");
			memcpy(var, serializado, sizeof(t_variable));
		//	printf("3AkkkkkSD\n");
			serializado += sizeof(t_variable);
		//	printf("4AkkkkkSD\n");
			memcpy(&var->direccion, serializado, sizeof(t_direccion));
			//printf("5AkkkkkSD\n");
			serializado += sizeof(t_direccion);
			//printf("6AooooSD\n");
			list_add(temp->vars, (void*)var);
		}
		//printf("ASD\n");
		list_add(pcb->contextoActual, (void*)temp);
	}
	//printf("RERE\n");

	return pcb;


}

void agregarContexto(t_pcb *pcb, t_contexto *contexto) {
	/*

	pcb->sizeContextoActual++;
	t_contexto **viejo;
	viejo = pcb->contextoActual;
	pcb->contextoActual = malloc(pcb->sizeContextoActual * sizeof(t_contexto*));


	memcpy(pcb->contextoActual, viejo, (pcb->sizeContextoActual - 1)*sizeof(t_contexto*));

	pcb->contextoActual[pcb->sizeContextoActual - 1] = contexto;

	free(viejo);

*/
}


t_blocked *desserializarBLOQUEO(char *serializado) {
	
	t_blocked *retorno;	t_pcb *pcb;
	int i=0;

	retorno = malloc(sizeof(t_blocked));
	memcpy(retorno, serializado, sizeof(t_blocked));
	serializado += sizeof(t_blocked);

	pcb = malloc(sizeof(t_pcb));
	pcb = desserializarPCB(serializado);
	//memcpy(pcb, serializado, sizeof(t_pcb));
	serializado += retorno->sizePcbSerializado;

	if (retorno->semaforoSize) {
		retorno->semaforo = malloc(retorno->semaforoSize);
		memcpy(retorno->semaforo , serializado, retorno->semaforoSize);
		serializado += retorno->semaforoSize;
	}


	if (retorno->ioSize) {

		retorno->io = malloc(retorno->ioSize);
		memcpy(retorno->io , serializado, retorno->ioSize);
		serializado += retorno->ioSize;
		
	}

	retorno->pcb=pcb;

	return (retorno);

}

char *serializarBLOQUEO(t_blocked *bloqueo) {
	char *retorno, *retornotemp; int size = 0;
	
	t_pcb *pcbSerializado;
	pcbSerializado = (t_pcb*)serializarPCB(bloqueo->pcb);

	bloqueo->sizePcbSerializado = pcbSerializado->sizeTotal;
	bloqueo->sizeTotal = (sizeof(char) * pcbSerializado->sizeTotal + sizeof(char) * (bloqueo->semaforoSize + bloqueo->ioSize) + sizeof(t_blocked));

	retorno = malloc(sizeof(char) * pcbSerializado->sizeTotal + sizeof(char) * (bloqueo->semaforoSize + bloqueo->ioSize) + sizeof(t_blocked));
	retornotemp=retorno;

	memcpy(retornotemp, bloqueo, sizeof(t_blocked));
	retornotemp += sizeof(t_blocked);

	memcpy(retornotemp, pcbSerializado, pcbSerializado->sizeTotal);
	retornotemp += pcbSerializado->sizeTotal;
	free(pcbSerializado);
	if (bloqueo->semaforoSize) {
		memcpy(retornotemp, bloqueo->semaforo, bloqueo->semaforoSize);
		retornotemp += bloqueo->semaforoSize;
	}


	if (bloqueo->ioSize) {
		memcpy(retornotemp, bloqueo->io, bloqueo->ioSize);
		//printf("%dEntreee%s\n",bloqueo->ioSize,retornotemp);
		retornotemp += bloqueo->ioSize;
	}


	return (retorno);

}


char *serializarPCB(t_pcb *pcb) {
	int g=0;


	int size = 0;
	char *retorno, *retornotemp, *retornotempp;

	size += sizeof(t_pcb);

	size += pcb->sizeIndiceDeEtiquetas * sizeof(char);;
	size += pcb->sizeIndiceDeCodigo * 2 * sizeof(int);;
	int i, y;



	//size += pcb->sizeContextoActual * sizeof(t_contexto*);
	for (i = 0; i < pcb->sizeContextoActual; i++) {
		size += sizeof(t_contexto);
		int y;
		t_contexto * contexto;
		contexto = list_get(pcb->contextoActual, i);


		for (y = 0; y < contexto->sizeArgs; y++) {
			size += sizeof(t_direccion);
		}
		for (y = 0; y < contexto->sizeVars; y++) {

			size += sizeof(t_variable);
			size += sizeof(t_direccion);
		}
	}

	retorno = malloc(size);
	retornotemp = retorno;
	pcb->sizeTotal = size;
	memcpy(retornotemp, pcb, sizeof(t_pcb));

	retornotemp += sizeof(t_pcb);


	memcpy(retornotemp, pcb->indiceDeCodigo, pcb->sizeIndiceDeCodigo * 2 * sizeof(int));
	retornotemp += pcb->sizeIndiceDeCodigo * 2 * sizeof(int);

	memcpy(retornotemp, pcb->indiceDeEtiquetas, pcb->sizeIndiceDeEtiquetas * sizeof(char));
	retornotempp = retornotemp;




	retornotemp += pcb->sizeIndiceDeEtiquetas * sizeof(char);

	//memcpy(retornotemp, pcb->contextoActual, pcb->sizeContextoActual * sizeof(t_contexto*));
	//retornotemp += pcb->sizeContextoActual * sizeof(t_contexto*);



	for (i = 0; i < pcb->sizeContextoActual; i++) {
		t_contexto *contexto;

		contexto = list_get(pcb->contextoActual, i);
		memcpy(retornotemp, contexto, sizeof(t_contexto));

		retornotemp += sizeof(t_contexto);

		for (y = 0; y < contexto->sizeArgs; y++) {
			t_direccion *dir;
			dir = list_get(contexto->args, y);
			memcpy(retornotemp, dir, sizeof(t_direccion));
			retornotemp += sizeof(t_direccion);
		}


		for (y = 0; y < contexto->sizeVars; y++) {
			t_variable *var;
			t_direccion *dir;

			var = list_get(contexto->vars, y);


			memcpy(retornotemp, var, sizeof(t_variable));


			retornotemp += sizeof(t_variable);
			memcpy(retornotemp, &var->direccion, sizeof(t_direccion));
			retornotemp += sizeof(t_direccion);

		}

	}

	//printf("SIZE%d\n",size);
	return retorno;
}
