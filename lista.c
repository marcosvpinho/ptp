#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

int main(void){
	node *LISTA = (node *) malloc(sizeof(node));
	if(!LISTA){
		printf("Sem memoria disponivel!\n");
		exit(1);
	}
	LISTA->prox = NULL;
	tam=0;
	void *a='a';
	insereInicio(LISTA,a,sizeof(a),1);
}
int vazia(node *LISTA){
	if(LISTA->prox == NULL) return 1;
	else return 0;
}
node *aloca(void * pkt, unsigned tam, int num){ //MUDAR...
	node *novo=(node *) malloc(sizeof(node));
	if(!novo){
		printf("Sem memoria disponivel!\n");
		exit(1);
	}else{
		novo->size=tam;
		novo->pkt=pkt;
		novo->num=num;
		return novo;
	}
}


void insereFim(node *LISTA,void * pkt, unsigned tam, int num){
	
	node *novo=aloca(pkt,tam,num);
	novo->prox = NULL;
	if(vazia(LISTA)) LISTA->prox=novo;
	else{
		node *tmp = LISTA->prox;
		while(tmp->prox != NULL) tmp = tmp->prox;
		tmp->prox = novo;
	}
	tam++;
}

void insereInicio(node *LISTA,void * pkt, unsigned tam, int num){
	node *novo=aloca(pkt,tam,num);
	node *oldHead = LISTA->prox;
	LISTA->prox = novo;
	novo->prox = oldHead;
	tam++;
}

void exibe(node *LISTA){
	system("clear");
	if(vazia(LISTA)){
		printf("Lista vazia!\n\n");
		return ;
	}
	
	node *tmp;
	tmp = LISTA->prox;
	printf("Lista:");
	while( tmp != NULL){
		printf("%5d", tmp->num);
		tmp = tmp->prox;
	}
	printf("\n        ");
	int count;
	for(count=0 ; count < tam ; count++)
		printf("  ^  ");
	printf("\nOrdem:");
	for(count=0 ; count < tam ; count++)
		printf("%5d", count+1);
	
		
	printf("\n\n");
}

void libera(node *LISTA){
	if(!vazia(LISTA)){
		node *proxNode,
			  *atual;
		
		atual = LISTA->prox;
		while(atual != NULL){
			proxNode = atual->prox;
			free(atual);
			atual = proxNode;
		}
	}
}
void insere(node *LISTA,void * pkt, unsigned tam, int num,int pos){//mudar posição int pos
	
	int count;
	if(pos>0 && pos <= tam){
		if(pos==1)
			insereInicio(LISTA,pkt,tam,num);
		else{
			node *atual = LISTA->prox,
				 *anterior=LISTA; 
			node *novo=aloca(pkt,tam,num);;
				 
			for(count=1 ; count < pos ; count++){
					anterior=atual;
					atual=atual->prox;
			}
			anterior->prox=novo;
			novo->prox = atual;
			tam++;
		}
			
	}else
		printf("Elemento invalido\n\n");		
}

node *retiraInicio(node *LISTA){
	if(LISTA->prox == NULL){
		printf("Lista ja esta vazia\n");
		return NULL;
	}else{
		node *tmp = LISTA->prox;
		LISTA->prox = tmp->prox;
		tam--;
		return tmp;
	}
				
}

node *retiraFim(node *LISTA){
	if(LISTA->prox == NULL){
		printf("Lista ja vazia\n\n");
		return NULL;
	}else{
		node *ultimo = LISTA->prox,
			 *penultimo = LISTA;
			 
		while(ultimo->prox != NULL){
			penultimo = ultimo;
			ultimo = ultimo->prox;
		}
			 
		penultimo->prox = NULL;
		tam--;
		return ultimo;		
	}
}

node *retira(node *LISTA){//MUDAR int opt...
	int opt,count;

	
	if(opt>0 && opt <= tam){
		if(opt==1)
			return retiraInicio(LISTA);
		else{
			node *atual = LISTA->prox,
				 *anterior=LISTA; 
				 
			for(count=1 ; count < opt ; count++){
				anterior=atual;
				atual=atual->prox;
			}
			
		anterior->prox=atual->prox;
		tam--;
		return atual;
		}
			
	}else{
		printf("Elemento invalido\n\n");
		return NULL;
	}
}
