#include "estructurasControl.h"


void destruirPCB(t_pcb *pcb){
	int i,y;
	for(i=0;i<pcb->sizeContextoActual;i++){
		for(y=0;y<pcb->contextoActual[i]->sizeArgs;y++){
			free(pcb->contextoActual[i]->args[y]);
		}
		for(y=0;y<pcb->contextoActual[i]->sizeVars;y++){
			free(pcb->contextoActual[i]->vars[y]);
		}
		free(pcb->contextoActual[i]);
	}
	free(pcb->contextoActual);
	free(pcb->indiceDeCodigo);
	free(pcb->indiceDeEtiquetas);
	free(pcb);
}



t_pcb *desserializarPCB(char *serializado){
	char  * serini = serializado;
	int temp;
	int size,i,y;
	t_pcb *pcb;

	pcb = malloc(sizeof(t_pcb));
	memcpy(pcb,serializado,sizeof(t_pcb));
	serializado+=sizeof(t_pcb);



	pcb->indiceDeCodigo = malloc(pcb->sizeIndiceDeCodigo*2*sizeof(int));
	memcpy(pcb->indiceDeCodigo,serializado,pcb->sizeIndiceDeCodigo*2*sizeof(int));
	serializado+=pcb->sizeIndiceDeCodigo*2*sizeof(int);
	
	pcb->indiceDeEtiquetas=malloc(pcb->sizeIndiceDeEtiquetas*sizeof(char));
	memcpy(pcb->indiceDeEtiquetas,serializado,pcb->sizeIndiceDeEtiquetas*sizeof(char));
	
	serializado+=pcb->sizeIndiceDeEtiquetas*sizeof(char);


	pcb->contextoActual=malloc(pcb->sizeContextoActual*sizeof(t_contexto*));
	memcpy(pcb->contextoActual,serializado,pcb->sizeContextoActual*sizeof(t_contexto*));
	serializado+=pcb->sizeContextoActual*sizeof(t_contexto*);




	for(i=0;i<pcb->sizeContextoActual;i++){
		pcb->contextoActual[i] = malloc(sizeof(t_contexto));
		
		memcpy(pcb->contextoActual[i],serializado,sizeof(t_contexto));	
	

		serializado+=sizeof(t_contexto);

		for(y=0;y<pcb->contextoActual[i]->sizeArgs;y++){
			pcb->contextoActual[i]->args[y] = malloc(sizeof(t_direccion));
			memcpy(pcb->contextoActual[i]->args[y],serializado,sizeof(t_direccion));		
			serializado+=sizeof(t_direccion);
		}

		for(y=0;y<pcb->contextoActual[i]->sizeVars;y++){
			pcb->contextoActual[i]->vars[y] = malloc(sizeof(t_variable));
			memcpy(pcb->contextoActual[i]->vars[y],serializado,sizeof(t_variable));		
			serializado+=sizeof(t_variable);
		}
	}

	return pcb;


}

void agregarContexto(t_pcb *pcb,t_contexto *contexto){
	pcb->sizeContextoActual++;
	t_contexto **viejo;
	viejo = pcb->contextoActual;
	pcb->contextoActual = malloc(pcb->sizeContextoActual*sizeof(t_contexto*));

	
	memcpy(pcb->contextoActual,viejo,(pcb->sizeContextoActual-1)*sizeof(t_contexto*));
	
	pcb->contextoActual[pcb->sizeContextoActual-1]=contexto;

	free(viejo);


}




char *serializarPCB(t_pcb *pcb){

	int size=0;
	char *retorno,*retornotemp,*retornotempp;

	size+=sizeof(t_pcb);

	size+=pcb->sizeIndiceDeEtiquetas*sizeof(char);;
	size+=pcb->sizeIndiceDeCodigo*2*sizeof(int);;
	int i,y;
	size+=pcb->sizeContextoActual*sizeof(t_contexto*);
	for(i=0;i<pcb->sizeContextoActual;i++){
		size+=sizeof(t_contexto);
		int y;
		for(y=0;y<pcb->contextoActual[i]->sizeArgs;y++){
			size+=sizeof(t_direccion);
		}
		for(y=0;y<pcb->contextoActual[i]->sizeVars;y++){

			size+=sizeof(t_variable);
		}
	}

	retorno = malloc(size);
	retornotemp=retorno;
	pcb->sizeTotal=size;
	memcpy(retornotemp,pcb,sizeof(t_pcb));
	
	retornotemp+=sizeof(t_pcb);

	memcpy(retornotemp,pcb->indiceDeCodigo,pcb->sizeIndiceDeCodigo*2*sizeof(int));
	retornotemp+=pcb->sizeIndiceDeCodigo*2*sizeof(int);

	memcpy(retornotemp,pcb->indiceDeEtiquetas,pcb->sizeIndiceDeEtiquetas*sizeof(char));
	retornotempp=retornotemp;

	retornotemp+=pcb->sizeIndiceDeEtiquetas*sizeof(char);



	memcpy(retornotemp,pcb->contextoActual,pcb->sizeContextoActual*sizeof(t_contexto*));
	retornotemp+=pcb->sizeContextoActual*sizeof(t_contexto*);

	for(i=0;i<pcb->sizeContextoActual;i++){
		memcpy(retornotemp,pcb->contextoActual[i],sizeof(t_contexto));	

		retornotemp+=sizeof(t_contexto);

		for(y=0;y<pcb->contextoActual[i]->sizeArgs;y++){
			memcpy(retornotemp,pcb->contextoActual[i]->args[y],sizeof(t_direccion));		
			retornotemp+=sizeof(t_direccion);
		}
		for(y=0;y<pcb->contextoActual[i]->sizeVars;y++){
			memcpy(retornotemp,pcb->contextoActual[i]->vars[y],sizeof(t_variable));		
			retornotemp+=sizeof(t_variable);
		}
	}

	return retorno;


}
