#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "filaTESTE.h"

//Uma fila do tipo FIFO ...
//inicia fila entrada e saida vindos da camada de rede
struct Fila * criaFila() {
  struct Fila * q = (struct Fila*)malloc(sizeof(struct Fila));

  q->inicio = 0;
  q->fim = 0;
  q->itens = 0;
 
  return q;
} 

int quant (struct Fila * q) {
  return q->itens;
}

int vazia(struct Fila * q) {
  return q->itens == 0;   
}

int cheia(struct Fila * q) {
  //printf("itens: %d \n MAX_ITEMS: %d \n\n", q->itens, MAX_ITEMS);	
  return q->itens == MAX_ITEMS; 
}


void enfileira(struct Fila * q, struct Pacote * pac) {
  if (cheia(q)) return;
  q->fila[q->fim] = pac;
  q->fim = (q->fim+1);
  q->itens ++;	
}

void printa(struct Fila * q){
    int i,k;
    k=0;
    for(i=q->inicio;k<quant(q);k++){         
        printf("%d, %s--",q->fila[i]->num,q->fila[i]->pkt);
        i++;        
    }
		printf("\n");

}

int retorna(struct Fila * q){
	int pos;
	pos=q->fila[(q->inicio)]->num;
	return pos;
}



struct Pacote * desenfileira(struct Fila * q) {
  struct Pacote * pac;

  if (vazia(q)) return NULL;
  pac = q->fila[q->inicio];
	destroiPacote(q->fila[q->inicio]);	
  q->inicio = (q->inicio + 1);
  q->itens--;
	

  return pac;
}



struct Pacote * criaPacote(void * data, int size,int seq) {
  struct Pacote * pac = (struct Pacote *)malloc(sizeof(struct Pacote));

  pac->num=seq;
  pac->size = size;
  pac->pkt = malloc(pac->size);
  memcpy(pac->pkt, data, pac->size);
  return pac;
}

void destroiPacote(struct Pacote * pac) {
  free(pac->pkt);
  free(pac);
}




