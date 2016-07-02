#include "estructurasControl.h"

void destruirPCB(t_pcb *pcb) {
	/*
	t_contexto *contexto_a_finalizar;
	while(pcb->sizeContextoActual != 0){
		contexto_a_finalizar= list_get(pcb->contextoActual, pcb->sizeContextoActual-1);
		while(contexto_a_finalizar->sizeVars != 0){
			t_direccion*temp=(((t_variable*)list_get(contexto_a_finalizar->vars, contexto_a_finalizar->sizeVars-1))->direccion);
			//	free(temp);
			//	free(list_get(contexto_a_finalizar->vars, contexto_a_finalizar->sizeVars-1));
				contexto_a_finalizar->sizeVars--;
			}
		while(contexto_a_finalizar->sizeArgs != 0){
				//	free((t_direccion*)list_get(contexto_a_finalizar->args, contexto_a_finalizar->sizeArgs-1));
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
*/
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
			list_add(temp->args, dir);
		}
	//	printf("SIZE var %d\n",temp->sizeVars);
		for (y = 0; y < temp->sizeVars; y++) {
		//	printf("AkkkkkSD\n");
			t_variable *var = malloc(sizeof(t_variable));
			//printf("1AkkkkkSD\n");
			t_direccion *dire =malloc(sizeof(t_direccion));

			//printf("2AkkkkkSD\n");
			memcpy(var, serializado, sizeof(t_variable));
		//	printf("3AkkkkkSD\n");
			serializado += sizeof(t_variable);
		printf("posicion %d \n",(serializado-serini));
		//dire=&(var->direccion);

			dire->offset=((int*)(serializado))[0];
			dire->pagina=((int*)(serializado))[2];
			dire->size=((int*)(serializado))[1];
			var->direccion = dire;

			printf("%d %d %d \n",dire->offset,dire->pagina,dire->size);
		//	memcpy((t_direccion*)(var->direccion), serializado, sizeof(t_direccion));
			//printf("5AkkkkkSD\n");
			serializado += sizeof(t_direccion);
			printf("RETORNA \n");
			list_add(temp->vars, var);
		}
		//printf("ASD\n");
		list_add(pcb->contextoActual, temp);
	}
	//printf("RERE\n");

	printf("DES TAMANIO PCB%d\n",(serializado-serini));
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


char *serializarPCB(t_pcb *pcb) {
	int g=0;

printf("Entre a serialziar\n");
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

printf("ACA DENO ENTERARa\n");

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
			t_direccion * diretemp = var->direccion;
			printf("ENCODING %d %d %d \n",diretemp->offset,diretemp->pagina,diretemp->size);

			memcpy(retornotemp,&diretemp->offset,4);
			memcpy(retornotemp+4,&diretemp->size,4);
			memcpy(retornotemp+8,&diretemp->pagina,4);

			//memcpy(retornotemp, diretemp, sizeof(t_direccion));
			printf("posicion %d \n",(retornotemp-retorno));


			retornotemp += sizeof(t_direccion);

		}

	}

	//printf("SIZE%d\n",size);
	return retorno;
}
