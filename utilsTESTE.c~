#include <ctype.h>
#include "filaTESTE.h"
#include "utilsTESTE.h" 


/**************************FUNCAO PARA DEPURACAO ******************************************************************/

void dump(char * buf, int n) {
  int i;
  char ascii[COLUNAS+1];
  ascii[COLUNAS] = 0;

  for (i=0; i < n; i++) {
    unsigned char x = buf[i];
    
    printf("%02X ", x);
    if (isprint(x) && ! isblank(x)) ascii[i % COLUNAS] = x;
    else ascii[i % COLUNAS] = '.';
    if (((i+1) % COLUNAS) == 0) printf("%s\n", ascii); 
  }
  i = i % COLUNAS;
  if (i > 0) {
    ascii[i] = 0;
    for (; i < COLUNAS; i++) printf("   ");
    printf("%s\n", ascii);
  } else puts("");
}

/************************* FUNÇÃO INTERFACE TUN ***********************************************************/

int tun_alloc(char *dev)
  {
      struct ifreq ifr;
      int fd, err;

      if( (fd = open("/dev/net/tun", O_RDWR)) < 0 ) {
         perror("");
         return  -1;
      }

      memset(&ifr, 0, sizeof(ifr));

      /* Flags: IFF_TUN   - TUN device (no Ethernet headers) 
       *        IFF_TAP   - TAP device  
       *
       *        IFF_NO_PI - Do not provide packet information  
       */ 
      ifr.ifr_flags = IFF_TUN | IFF_NO_PI; 
      if( *dev )
         strncpy(ifr.ifr_name, dev, IFNAMSIZ);

      err = ioctl(fd, TUNSETIFF, (void *) &ifr);
      if( err < 0 ){
         close(fd);
	 perror("");
         return err;
      }
      strcpy(dev, ifr.ifr_name);
      return fd;
}             
 
/************************* Configuração da TUN ***********************************************************/

int set_ip(char *dev, char * ip, char * dst) {
  struct ifreq ifr;
  struct sockaddr_in *addr;
  int ok;
  int sd;
 
  // cria um socket para configurar a interface
  // esse socket não tem nada de especial ... 
  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sd < 0) return sd;
 
  // zera todos os bytes da struct ifreq
  // essa struct contém os atributos a serem configurados na interface
  bzero(&ifr, sizeof(ifr));
 
  // usa o ponteiro addr para referenciar o campo de endereço da struct ifreq
  // isso facilita o preenchimento dos atributos desse campo
  addr = (struct sockaddr_in*)&(ifr.ifr_ifru.ifru_addr);
 
  // preenche o campo endereço com o enderço IP da interface
  addr->sin_addr.s_addr = inet_addr(ip);
  addr->sin_family = AF_INET;
  addr->sin_port = 0;
 
  // escreve o nome da interface na struct ifreq. Isso é necessário
  // para o kernel saber que interface é alvo da operação
  strncpy(ifr.ifr_name, dev, IFNAMSIZ);
 
  // executa uma operação de configuração de endereço IP de interface
  ok = ioctl(sd, SIOCSIFADDR, &ifr);
  if (ok < 0) return ok;
 
  // preenche o campo endereço com o endereço IP da outra ponta do enlace
  addr->sin_addr.s_addr = inet_addr(dst);
 
  // executa uma operação de configuração de endereço IP de destino da interface  
  ok = ioctl(sd, SIOCSIFDSTADDR, &ifr);
  if (ok < 0) return ok;
 
  // preenche o campo endereço com a máscara de rede da interface
  addr->sin_addr.s_addr = inet_addr(MASK);
 
 
  // executa uma operação de configuração de máscara de rede da interface  
  ok = ioctl(sd, SIOCSIFNETMASK, &ifr);
  if (ok < 0) return ok;
 
  // executa uma operação de configuração de MTU da interface  
  ifr.ifr_mtu = MTU;
  ok = ioctl(sd, SIOCSIFMTU, &ifr);
  if (ok < 0) return ok;
 
  // lê as flags da interface
  ok = ioctl(sd, SIOCGIFFLAGS, &ifr);
  if (ok < 0) return ok;
 
  // acrescenta flags UP (ativa) e RUNNING (pronta para uso)
  ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
 
  // executa uma operação de configuração flags da interface  
  return ioctl(sd, SIOCSIFFLAGS, &ifr);
}




