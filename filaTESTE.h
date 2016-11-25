#ifndef FILATESTE_H
#define FILATESTE_H

struct Pacote {
  void * pkt;
  int num;
  unsigned int size;
};

#define MAX_ITEMS 1000

struct Fila {
  struct Pacote * fila[MAX_ITEMS];
  //int  num[1000];//antigo
  int inicio, fim, itens;
};
struct Fila * criaFila();
void enfileira(struct Fila * q, struct Pacote * pac);
struct Pacote * desenfileira(struct Fila * q);
int vazia(struct Fila * q);
int cheia(struct Fila * q);
int quant(struct Fila * q);
void printa(struct Fila * q);
int retorna(struct Fila * q);

struct Pacote * criaPacote(void * data, int size,int seq);
void destroiPacote(struct Pacote * pac);

#endif
