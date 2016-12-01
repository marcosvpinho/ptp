#define LISTA_H

#include <stdio.h>
#include <stdlib.h>

struct Node{
	int num;
	void * pkt;
  unsigned int size;
	struct Node *prox;
}; 
typedef struct Node node;

int tam;
node *aloca(void * pkt, unsigned tam, int num);
void insereFim(node *LISTA,void * pkt, unsigned tam, int num);
void insereInicio(node *LISTA,void * pkt, unsigned tam, int num);
void exibe(node *LISTA);
void libera(node *LISTA);
void insere (node *LISTA,void * pkt, unsigned tam, int num,int pos);
node *retiraInicio(node *LISTA);
node *retiraFim(node *LISTA);
node *retira(node *LISTA);
