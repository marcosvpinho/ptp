//gcc -o noack noack1.c $(pkg-config --cflags --libs libnl-genl-3.0) 
// ./noack

#include <errno.h>
#include <netlink/genl/genl.h>
#include "nl80211.h"
#include <stdio.h>
#include <stdlib.h>


int set_tx_queue_params(struct nl_sock *socket, int if_index, int driver_id) {

#ifdef NL80211_ATTR_WIPHY_TXQ_PARAMS

	uint16_t txop;
	uint16_t cw_min = 0x01;
	uint16_t cw_max = 0x01;
	uint8_t  aifs   = 0x00;
	uint8_t	 queue; // identificador de fila tx;

	struct nl_msg *msg;
	struct nlattr *txq;
	struct nlmsghdr *nlh;	

	msg = nlmsg_alloc();
	
	nlh = genlmsg_put(msg, 0, 0, driver_id, 0, 0, NL80211_CMD_SET_WIPHY, 0);
	NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, if_index); 
	txq = nla_nest_start(msg, NL80211_ATTR_WIPHY_TXQ_PARAMS);	

	NLA_PUT_U8(msg, NL80211_TXQ_ATTR_QUEUE, queue);
	NLA_PUT_U16(msg, NL80211_TXQ_ATTR_TXOP, txop);
	NLA_PUT_U16(msg, NL80211_TXQ_ATTR_CWMIN, cw_min);
	NLA_PUT_U16(msg, NL80211_TXQ_ATTR_CWMAX, cw_max);
	NLA_PUT_U8(msg, NL80211_TXQ_ATTR_AIFS, aifs);

	nla_nest_end(msg, txq);

	int enviar = nl_send_auto_complete(socket, msg); // envia a mensagem
	printf("Enviado NL80211_CMD_SET_WIPHY %d bytes para o kernel.\n", enviar);
		
	printf("burst time: %d\n", txop);
	printf("cwmin: %d\n", cw_min);
	printf("cwmax: %d\n", cw_max);
	printf("aifs: %d\n", aifs);
	printf("queue: %d\n", queue);
	printf("-----------------------------------------------------------------\n");	
	
	nlmsg_free(msg);

	return 0;
 nla_put_failure:
	return -ENOBUFS;


 #endif /* NL80211_ATTR_WIPHY_TXQ_PARAMS */

}

int main(int argc, char **argv) {

	uint16_t noack_map = 0xffff; // preenche noack_map com bits em 1.
	struct nlmsghdr *nlh;

	int if_index = if_nametoindex("wlan1"); // usar a interface especificada
	struct nl_sock *socket = nl_socket_alloc(); // atribui um socket
	genl_connect(socket); // conecta no socket
	int driver_id = genl_ctrl_resolve(socket, "nl80211"); // encontra o ID.
	
	int erro = set_tx_queue_params(socket, if_index, driver_id);
	if (erro != 0) {
     	   printf("set_tx_queue_params() falhou com %d.\n", erro);
           return erro;
        }
	
	struct nl_msg *msg = nlmsg_alloc(); // atribiu uma mensagem.
	nlh = genlmsg_put(msg, 0, 0, driver_id, 0, NLM_F_CREATE, NL80211_CMD_SET_NOACK_MAP, 0); // cria um cabeçalho da mensagem generic netlink; NLM_F_CREATE: cria um comando. 
	nla_put_u32(msg, NL80211_ATTR_IFINDEX, if_index);  // add atributo na mensagem, qual interface usar.
	NLA_PUT_U16(msg, NL80211_ATTR_NOACK_MAP, noack_map); //adiciona os dados na mensagem.
	int enviar = nl_send_auto_complete(socket, msg); // envia a mensagem
	printf("Enviado NL80211_CMD_SET_NOACK_MAP %d bytes para o kernel.\n", enviar);
	
	printf("noack_map: %d\n", noack_map);
	printf("if_index: %d\n", if_index);
	printf("id: %d\n", driver_id);

	nlmsg_free(msg);
	nl_socket_free(socket);

	return 0;
 nla_put_failure:
	return -ENOBUFS;

}

