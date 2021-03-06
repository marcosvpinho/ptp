#include <sys/select.h>
#include <stdlib.h>
#include <stdio.h>
#include "utilsTESTE.h"
#include "filaTESTE.h"

// duração do token em milissegundos
#define TOKEN_MS 200
#define Mestre 1
#define Escravo 0
#define MaxTimeouts 2

enum Estados {Recepcao, Transmissao};

  int estado = 0; 
  int cont = 0;
  int poll = 0;
  
  int sequence=10;
  char ind[4];
	
  unsigned char src_mac[6]; 
  unsigned char dest_mac[6] = {0xe8, 0x94, 0xf6, 0x09, 0xb1, 0xd2};

 
  struct sockaddr_ll socket_address;
  struct sockaddr_ll socket_address_token; 

/******************** INICIA O LINK DA TAP E DO SOCKET*******************************************************/

int inicia_link(struct Link * link, char * interface_wlan, char * modo) {
// Cria um socket, que é vinculado com a interface sem fio
   
  // link->fifoOut = criaFila();

   link->sd = cria_socket_dados(interface_wlan);
   if (link->sd < 0) {
     perror("ao abrir o socket de dados");
     return 0;
   }

   if (modo[0] == 'e' || modo[0] == 'E') link->modo = Escravo;
   else if (modo[0] == 'm' || modo[0] == 'M') link->modo = Mestre;
   else {
     printf("Modo inválido: %c\n", modo[0]);
     close(link->sd);
     return 0;
   }
  
   link->token = cria_socket_token(interface_wlan);
   if (link->token < 0) {
     perror("ao abrir o socket do token");
     return 0;
   }

   
   // Cria a interface tun, que representa a camada de rede
   link->nome[0] = 0;
   link->net = tun_alloc(link->nome);
   if (link->net < 0) {
     perror("");
     close(link->sd);
     //close(link->token);
     return 0;
   }	

   return 1;	
}

/************************* FUNÇÃO CRIA UM SOCKET **************************************************************/

//Obtém o número de uma interface de rede, conforme registrada no sistema operacional
int getif_dados(int sd, char * ifname) {
  struct ifreq ifr;

  strcpy(ifr.ifr_name, ifname);
  if (ioctl(sd, SIOCGIFINDEX, &ifr) < 0) {
    perror("interface ???");
    return -1;
  }
  int i;
  for (i=0;i<=5;i++) {
	src_mac[i] = ifr.ifr_hwaddr.sa_data[i];
  }
  return ifr.ifr_ifindex;
}

// Cria um socket e vincula com a interface de rede wlan
int cria_socket_dados(char * ifname) { //retirado o parametro
  //struct sockaddr_ll addr;
  int sd;
  //struct sockaddr_ll socket_address;
  // criação de um socket do tipo AF_PACKET, que captura quadros recebidos pela camada
  // de enlace.IP
  if ((sd = socket(AF_PACKET, SOCK_DGRAM, htons(0x0800))) < 0) {
    perror("Ao criar o socket");
    return -1;
  }

  printf("Abriu socket de dados com sucesso: %i\n", sd); 
  // Precisa vincular o socket a uma interface de rede, usando uma "struct sockaddr_ll"
  socket_address.sll_family   = AF_PACKET;
  socket_address.sll_protocol = htons(0x0800);
  socket_address.sll_ifindex  = getif_dados(sd, ifname); // descobre o numero da interface de rede
  socket_address.sll_halen    = ETH_ALEN;
      
  if (socket_address.sll_ifindex < 0) return -1;

  // vincula o socket a interface de rede
  if (bind(sd, (struct sockaddr*)&socket_address, sizeof(socket_address)) < 0) {
    perror("Ao fazer bind");
    return -1;
  }
 
  return sd;
}

/************************* FUNÇÃO CRIA UM SOCKET PARA O TOKEN **************************************************************/

//Obtém o número de uma interface de rede, conforme registrada no sistema operacional
int getif_token(int token, char * ifname) {
  struct ifreq ifr;

  strcpy(ifr.ifr_name, ifname);
  if (ioctl(token, SIOCGIFINDEX, &ifr) < 0) {
    perror("interface ???");
    return -1;
  }
  int i;
  for (i = 0; i < 6; i++) {
    src_mac[i] = ifr.ifr_hwaddr.sa_data[i]; 
  } 
  
  return ifr.ifr_ifindex;
}

// Cria um socket e vincula com a interface de rede wlan
int cria_socket_token(char * ifname) { //retirado o parametro
  int token;
 
  // criação de um socket do tipo AF_PACKET, que captura quadros recebidos pela camada
  // de enlace.
  if ((token = socket(AF_PACKET, SOCK_DGRAM, htons(0x080a))) < 0) {
    perror("Ao criar o socket");
    return -1;
  }
  

  printf("Abriu socket para o TOken com sucesso: %i\n", token); 
  // Precisa vincular o socket a uma interface de rede, usando uma "struct sockaddr_ll"
  socket_address_token.sll_family   = AF_PACKET;
  socket_address_token.sll_protocol = htons(0x080a);
  socket_address_token.sll_ifindex  = getif_token(token, ifname); // descobre o numero da interface de rede
  
  if (socket_address_token.sll_ifindex < 0) return -1;

  // vincula o socket a interface de rede
  if (bind(token, (struct sockaddr*)&socket_address_token, sizeof(socket_address_token)) < 0) {
    perror("Ao fazer bind");
    return -1;
  }
 
  return token;
}

/***********************TRANSMISSÃO E RECEPÇÃO ***************************************/

int envia_poll(struct Link *link) {
   
   void * bufferPoll = NULL;
   unsigned char* eth_head;	// ponteiro byte para o cabeçalho ethernet
   unsigned char* eth_data;	// ponteiro byte para campo de dados ethernet
   struct ethhdr *eh;       // ponteiro struturado para cabeçalho ethernet
   int i; 
   
   //socket_address.sll_family   = AF_PACKET;
   //socket_address.sll_ifindex  = getif(sd, ifname); // descobre o numero da interface de rede
   socket_address_token.sll_protocol = htons(0x080a);
   //socket_address.sll_ifindex  = ifindex;
   // socket_address.sll_pkttype  = PACKET_HOST;
   socket_address_token.sll_halen    = ETH_ALEN;
   socket_address_token.sll_addr[0]  = dest_mac[0];
   socket_address_token.sll_addr[1]  = dest_mac[1];
   socket_address_token.sll_addr[2]  = dest_mac[2];
   socket_address_token.sll_addr[3]  = dest_mac[3];
   socket_address_token.sll_addr[4]  = dest_mac[4];
   socket_address_token.sll_addr[5]  = dest_mac[5];
   socket_address_token.sll_addr[6]  = 0x00; 
   socket_address_token.sll_addr[7]  = 0x00;

   //printa(link->sequencia);
   // Prepara estrutura de dados 
   // Ethernet frame: Target MAC (6)+ Src MAC(6) + Type (6)+ Data(+Padd)(46..1500) + CRC (4) 
   bufferPoll = (void*)malloc(ETHER_MAX_LEN);  
   eth_head = bufferPoll;                     
   eth_data = bufferPoll + ETH_HLEN;          
   eh = (struct ethhdr *)eth_head;

   memcpy((void *)eh->h_dest,(void*)dest_mac, ETH_ALEN);
   memcpy((void *)eh->h_source,(void*)src_mac, ETH_ALEN);
   eh->h_proto = htons(0x080a);
	  

	
   i = sendto(link->token, bufferPoll, D_SIZE+ETHER_HDR_LEN, 0, (struct sockaddr*)&socket_address_token, sizeof(socket_address_token));
   if (i == -1) {
     perror("sendto():");
     return -1;
   } else {
    // printf("Enviando Poll (%d bytes)\n", i);
     estado = 0; 
   }
}


int recebe_token (struct Link * link) {

    unsigned char* eth_head;	// ponteiro byte para o cabeçalho ethernet
    unsigned char* eth_data;	// ponteiro byte para campo de dados ethernet
    struct ethhdr *eh;          // ponteiro struturado para cabeçalho ethernet
    int bytesToken;
    void * bufferToken = NULL; 
    int buf[MAX_BUF];

    bufferToken = (void*)malloc(ETHER_MAX_LEN);  
    eth_head = bufferToken;                     
    eth_data = bufferToken + ETH_HLEN;          
    eh = (struct ethhdr *)eth_head;
     
    socket_address_token.sll_protocol = htons(0x080a);
    socket_address_token.sll_pkttype  = PACKET_OTHERHOST;

    socklen_t alen = sizeof(socket_address_token);
    bytesToken = recvfrom(link->token, buf, ETHER_MAX_LEN, 0, (struct sockaddr*)&socket_address_token, &alen);
	int i,y;
	int valor[bytesToken/4];
	for(i=0;i<bytesToken/4;i++)valor[i]=buf[i];
	int v[100];
	int q=0,flag=0;
	for(y=0;y<bytesToken/4;y++){
		if((y%2)==0 && y>0){
			i=valor[y]-valor[y-1]-1;
			int xs = valor[y-1];
			for(;i>0;i--){
				xs=xs+1;
				v[q]=xs;
				q++;	
				printf("%d",v[q]);
			}
		}
	}
	/*
	struct Link link2;         
	link2.sequencia=criaFila();
    for(i=0;i<quant(link->sequencia);i++){
		if(retorna(link->sequencia)==v[flag]){
	 		//enfileira(link2.sequencia,desenfileira(link->sequencia));
			flag++;
		}
		//else desenfileira(link->sequencia);	
	}
	//printa(link2.sequencia);
	*/
   if(bytesToken < 0) {
     //perror("ao receber o Token");
     return -1;
   }
   if(eh->h_proto == ntohs(0x080a)) {
     //printf("recebeu Poll++++++++++++++++++++++++++++++++++++++!\n"); 
     //write(link->net, bufferToken, bytesToken); 
     poll = 1;
     estado = 1; 
   } else {
     estado = 0;
   }
}
int conta(){
	if(sequence==300000) sequence=0;
	sequence++;
	return sequence;
}

int recebe_da_Wlan(struct Link *link) {
   unsigned char* eth_head;	// ponteiro byte para o cabeçalho ethernet
   unsigned char* eth_data;	// ponteiro byte para campo de dados ethernet
   struct ethhdr *eh;            // ponteiro struturado para cabeçalho ethernet
   int bytesWlan;
   char bufferWlan[MTU];

   //socket_address.sll_protocol = htons(ETH_P_ALL);
   //socket_address.sll_pkttype  = PACKET_OTHERHOST;

   socklen_t alen = sizeof(socket_address);
   bytesWlan = recvfrom(link->sd, bufferWlan, MTU, 0, (struct sockaddr*)&socket_address, &alen);
   if(bytesWlan < 0) {
     perror("ao receber da WLAN");
   } else {
    char seq[4];
    seq[0]=bufferWlan[0];
    seq[1]=bufferWlan[1];
    seq[2]=bufferWlan[2];
    seq[3]=bufferWlan[3];
    
     //printf("Pacote numero: %s\n",seq);
     int envia_tun = write(link->net, bufferWlan, bytesWlan); 


     if(envia_tun < 0) {
       perror("enviar TUN");
     } else {
       //printf("enviado para TUN (%d bytes)!!\n", envia_tun);   
     }    
   }  
} 

void ativa_link(struct Link * link) {
    fd_set readfds, writefds;
    char bufferTUN[MAX_BUF]; // buffer para guardar uma PDU vinda da camada de rede (TUN).
    //char bufferWlan[MTU];
    unsigned char* eth_head;	// ponteiro byte para o cabeçalho ethernet
    unsigned char* eth_data;	// ponteiro byte para campo de dados ethernet
    struct ethhdr *eh;  
    struct timeval timeout, baseTimeout;
    int maior_fd = link->sd;
    int num_timeouts = 0;	
    estado = (link->modo == Mestre);
    printf("estado=%d\n", estado);

    if (link->token > maior_fd) maior_fd = link->token;
    //if (link->net > maior_fd) maior_fd = link->net;

    if(fcntl(link->net, F_SETFL, O_NONBLOCK) < 0) {
        perror("no fcntl");
        return;   
     } 

    baseTimeout.tv_sec = TOKEN_MS / 1000;
    baseTimeout.tv_usec = 1000*(TOKEN_MS % 1000);
    //envia_poll(link);
    timeout = baseTimeout;
 
    while(1) {
        switch(estado) {
        case 0: // recepção
        FD_ZERO(&readfds);
        FD_SET(link->sd, &readfds);
        FD_SET(link->token, &readfds);
        int n = select(maior_fd+1, &readfds, NULL, NULL, &timeout);
        if(n) {
            if(FD_ISSET(link->sd, &readfds)) {
	            recebe_da_Wlan(link);	
            }
            if(FD_ISSET(link->token, &readfds)) {
	            recebe_token(link);
                timeout = baseTimeout;
            }
        }else{ // timeout !!
            if (timeout.tv_sec == 0 && timeout.tv_usec == 0) {
            // recarregar o timeout ... talvez token se perdeu ??? 
            // recuperação do token deve ser feita numa nova versão ...
            timeout = baseTimeout;
            num_timeouts ++;
            if (num_timeouts > MaxTimeouts && link->modo == Mestre) estado = 1;
            }
        } 
        break;
        case 1: // transmissão
        FD_ZERO(&readfds);
        FD_SET(link->net, &readfds);

        //printf("---Aguardando algo em stdin por até %ld.%ld segundos...\n>>> ", timeout.tv_sec, timeout.tv_usec);
        fflush(stdout);	

        n = select(link->net+1, &readfds, NULL, NULL, &timeout);

        if(n) {
            uint8_t bufferSeq[MAX_BUF+4];
            int bytesTUN = read(link->net, bufferSeq+4, MAX_BUF);
            if(bytesTUN < 0 ){
		        perror("ao receber da tun");
            } //else printf("Chegou algo da camada de rede (%d bytes)...\n", bytesTUN);
	        
       	    int seq=conta();
	 	    bufferSeq[0] = (seq & 0x000000ff);
    	    bufferSeq[1] = (seq & 0x0000ff00) >> 8;
    		bufferSeq[2] = (seq & 0x00ff0000) >> 16;
    	    bufferSeq[3] = (seq & 0xff000000) >> 24;            

             socket_address.sll_halen    = ETH_ALEN;
             // memcpy(socket_address.sll_addr, dest_mac, ETH_ALEN);
             socket_address.sll_addr[0]  = dest_mac[0];
             socket_address.sll_addr[1]  = dest_mac[1];
             socket_address.sll_addr[2]  = dest_mac[2];
             socket_address.sll_addr[3]  = dest_mac[3];
             socket_address.sll_addr[4]  = dest_mac[4];
             socket_address.sll_addr[5]  = dest_mac[5];
             socket_address.sll_addr[6]  = 0x00; 
             socket_address.sll_addr[7]  = 0x00;    
	
             int bytesWlan = sendto(link->sd, bufferSeq, bytesTUN+4, 0, (struct sockaddr*)&socket_address, sizeof(socket_address));

             //enfileirando dados que foram enviados
             struct Pacote *packet;
             packet=criaPacote(bufferTUN,bytesTUN,seq);
             enfileira(link->sequencia,packet);


             if(bytesWlan < 0) {
                perror("ao enviar para WLAN");
            }else {
	   				//printf("Enviados %d bytes pela interface Wlan com sucesso!!!\n", bytesWlan);
            }
       }
       if((timeout.tv_sec == 0) && (timeout.tv_usec == 0)) {
         //puts("Atingiu timeout!");
         num_timeouts = 0;
         envia_poll(link);
       } 
       break;
   }
 }
}  




int main(int argc, char ** argv) {
	struct Link link;         
	link.sequencia=criaFila();
	//Testa se foram fornecidos 2 argumentos na linha de comando
	if (argc < 3) {
		printf("Uso: %s nome_iface_wlan {m|e}\n", argv[0]);
	    printf("\nnome_iface_wlan: nome da interface wlan\n");
        printf("m|e: m=modo mestre, e=modo escravo\n");
        printf("\nExemplo de execução em modo mestre: %s wlan2 m\n", argv[0]);
	    return 0;
	}         
    if (! inicia_link(&link, argv[1], argv[2])) {
        printf("Falhou ao iniciar o enlace ... abortando\n");
        return  1;
    }
	if (set_ip("tun0", "10.1.1.3", "10.1.1.1") < 0) {
        perror("ao configurar a interface tun");
        return 0;
    }
     
    // Ativa o seu enlace ...
    ativa_link(&link);
	
    close(link.token); 
	close(link.sd);
    close(link.net);
	return 0;
}
