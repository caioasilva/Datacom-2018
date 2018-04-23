#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>

#define BUFFER_SIZE 1024

struct dataProtocol{
	char sourceName[10];
	char destinationName[10];
	char* message;
	unsigned int size=0;
};

int main(int argc, char *argv[])
{
	int sockfd;
	struct ifreq if_idx;
	struct ifreq if_mac;
	int tx_len = 0;
	char sendbuf[BUF_SIZ];
	struct ether_header *eh = (struct ether_header *) sendbuf;
	struct iphdr *iph = (struct iphdr *) (sendbuf + sizeof(struct ether_header));
	struct sockaddr_ll socket_address;
	char interfaceName[IFNAMSIZ];
	uint8_t destinationMAC[6];
	char sourceName[10];
	char destinationName[10];
	char 
	
	/* Get interface name */
	if (argc > 5)
		strcpy(interfaceName, argv[1]);
      	sscanf(argv[2], "%02x:%02x:%02x:%02x:%02x:%02x", &destinationMAC[0], &destinationMAC[1], &destinationMAC[2], &destinationMAC[3], &destinationMAC[4], &destinationMAC[5]);
      	sscanf(argv[3], "%9s", &sourceName);
      	sscanf(argv[4], "%9s", &destinationName);
      	sscanf(argv[5], "%9s", &sourceName);

	else
		strcpy(interfaceName, DEFAULT_INTERFACE);

	/* Open RAW socket to send on */
	if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
	    perror("socket");
	}

	/* Get the index of the interface to send on */
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, interfaceName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
	    perror("SIOCGIFINDEX");
	/* Get the MAC address of the interface to send on */
	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, interfaceName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
	    perror("SIOCGIFHWADDR");

	/* Construct the Ethernet header */
	memset(sendDataBuffer, 0, BUFFER_SIZE);

	/* Ethernet header */
	eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
	eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
	eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
	eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
	eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
	eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
	eh->ether_dhost[0] = destinationMAC[0];
	eh->ether_dhost[1] = destinationMAC[1];
	eh->ether_dhost[2] = destinationMAC[2];
	eh->ether_dhost[3] = destinationMAC[3];
	eh->ether_dhost[4] = destinationMAC[4];
	eh->ether_dhost[5] = destinationMAC[5];

	/* Ethertype field */
	eh->ether_type = htons(0x1996);//htons(ETH_P_IP);
	tx_len += sizeof(struct ether_header);

	struct dataProtocol msgData;
	msgData.source

	/* Packet data */
	sendbuf[tx_len++] = 0xde;
	sendbuf[tx_len++] = 0xad;
	sendbuf[tx_len++] = 0xbe;
	sendbuf[tx_len++] = 0xef;

	/* Index of the network device */
	socket_address.sll_ifindex = if_idx.ifr_ifindex;
	/* Address length*/
	socket_address.sll_halen = ETH_ALEN;
	/* Destination MAC */
	memcpy (socket_address.sll_addr, eh->ether_dhost, 6);


	/* Send packet */
	if (sendto(sockfd, sendbuf, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
	    printf("Send failed\n");
		for (int j = 0; j< tx_len;j++)
			printf("%x ",sendbuf[j]);

	return 0;
}