#ifndef UTILSTESTE_H
#define UTILSTESTE_H

#include <sys/types.h>
#include <strings.h>
#include <unistd.h> //fechar o socket
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <net/if.h>
#include <asm/ioctl.h>
#include <string.h>
#include <linux/if_tun.h>
#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <linux/if_ether.h>

#include <sys/time.h> 
#include <ctype.h>

#include <signal.h>
#include <sys/ioctl.h>

//#include <linux/if.h>
#include <asm/types.h>
#include <fcntl.h>

#define MAXIMO 4096  
#define FIFO_BLOCO 2048
#define COLUNAS 15

//#define DEVICE "wlan2"
#define MTU 1496
#define MASK "255.255.255.0"
#define MAX_BUF 2048
//#define ETH_TYPE_FIELD 	0x080a
#define D_SIZE 10 /* Tamanho do dado da mensagem */


/* Uma fila do tipo FIFO ...
struct Fifo {
  char buffer[MAXIMO];
  int bytes;
  int inicio, fim;
};*/

// A struct Link descreve a interface tap,
// que representa a camada de rede.
// Ela contem  tambem um buffer do tipo FIFO, usado para
// guardar os bytes vindos da tap.


struct Link {
  int sd; // o descritor da interface sem fio
  int net; // o descritor da camada de rede
  int token; // descritor do POll
  char nome[IFNAMSIZ]; // nome da interface tap
  int modo; // 1=mestre, 0=escravo
  //struct sockaddr_ll socket_address;

  //struct Fila * fifo; // o buffer para receber o que vem da interface wlan,
             // contendo os bytes jah recebidos e ainda nao processados
             // pelo protocolo de enlace
  struct Fila * sequencia; // o buffer dos bytes a serem transmitidos pelo socket para interface wlan,
             // que constituem o quadro gerado pelo protocolo de enlace
  
};

// Funcao para iniciar uma struct Link
// interface_wlan contem o nome da interface sem fio ("wlanX").
// No argumento "link" serao guardados os descritores da interface sem fio,
// da interface tap (camada de rede), e o nome da interface tap,
// que eh gerado automaticamente pelo sistema operacional.
// Retorna 0 se falhar, e 1 se tiver sucesso.

int inicia_link(struct Link * link, char * interface_wlan, char * modo);

int tun_alloc(char *dev);

int getif_dados(int sd, char * ifname);

int cria_socket_dados(char * ifname);

int getif_token(int token, char * ifname);

int cria_socket_token(char * ifname);

int fd_set_blocking(int fd, int blocking);

// Funcao para mostrar o conteudo de um quadro ...
// Apenas mostra na tela seus bytes em hexadecimal.
// Usada para depuracao (se necessario).

void dump(char * buf, int n);

int envia_poll(struct Link* link); 
int recebe_da_Wlan(struct Link* link);
int recebe_token(struct Link* link);

#endif















