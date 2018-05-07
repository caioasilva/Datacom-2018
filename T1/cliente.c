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
#define SOURCE_NAME_SIZE 10
#define DEST_NAME_SIZE 10
#define ETHER_TYPE	0x1996

char* encodeProtocol(int* size, char* destinationName, char* sourceName, char* message)
{	
	//printf("%s",message);
	*size = SOURCE_NAME_SIZE + DEST_NAME_SIZE + strlen(message);

	//char data[size];
	char* data = calloc(*size,sizeof(char));
	char* prot_ptr = data;
	memcpy(prot_ptr,destinationName,strlen(destinationName));
	prot_ptr+=DEST_NAME_SIZE;
	memcpy(prot_ptr,sourceName,strlen(sourceName));
	prot_ptr+=SOURCE_NAME_SIZE;
	memcpy(prot_ptr,message,strlen(message));
	return data;
}


int main(int argc, char *argv[])
{
	int sockfd;
	struct ifreq if_idx;
	struct ifreq if_mac;
	int tx_len = 0;
	char sendDataBuffer[BUFFER_SIZE];
	struct ether_header *eh = (struct ether_header *) sendDataBuffer;
	struct iphdr *iph = (struct iphdr *) (sendDataBuffer + sizeof(struct ether_header));
	struct sockaddr_ll socket_address;
	char interfaceName[IFNAMSIZ];
	unsigned int destinationMAC[6];
	//char sourceName[SOURCE_NAME_SIZE];
	//char destinationName[DEST_NAME_SIZE];
	//char* message; 
	char* protocol;
	int protocol_size;
	
	/* Get interface name */
	if (argc > 5){
		strcpy(interfaceName, argv[1]);
      	sscanf(argv[2], "%02x:%02x:%02x:%02x:%02x:%02x", &destinationMAC[0], &destinationMAC[1], &destinationMAC[2], &destinationMAC[3], &destinationMAC[4], &destinationMAC[5]);
      	//strncpy(sourceName, argv[3], SOURCE_NAME_SIZE);
      	//strncpy(destinationName, argv[4], DEST_NAME_SIZE);
      	//message = malloc(sizeof(argv[5])*sizeof(char)+1);
      	//strcpy(message, argv[5]);
      	protocol = encodeProtocol(&protocol_size, argv[4],argv[3],argv[5]);
      	//protocol_size = sizeof(protocol);

	}else{
		fprintf(stderr,"Invalid arguments\n");
		exit(EXIT_FAILURE);
	}

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
	eh->ether_type = htons(ETHER_TYPE);//htons(ETH_P_IP);
	tx_len += sizeof(struct ether_header);


	// char* prot_ptr = sendDataBuffer+tx_len;
	// memcpy(prot_ptr,destinationName,DEST_NAME_SIZE);
	// prot_ptr+=DEST_NAME_SIZE;
	// memcpy(prot_ptr,sourceName,SOURCE_NAME_SIZE);
	// prot_ptr+=SOURCE_NAME_SIZE;
	// memcpy(prot_ptr,message,strlen(message));
 //    tx_len += (SOURCE_NAME_SIZE + DEST_NAME_SIZE + strlen(message) + 1);
	//printf("%d, %s\n",tx_len,sendDataBuffer);
	//printf("%d, %s\n",protocol_size, protocol);

	memcpy(sendDataBuffer+tx_len,protocol,protocol_size);
	tx_len+=protocol_size;

	/* Index of the network device */
	socket_address.sll_ifindex = if_idx.ifr_ifindex;
	/* Address length*/
	socket_address.sll_halen = ETH_ALEN;
	/* Destination MAC */
	memcpy (socket_address.sll_addr, eh->ether_dhost, 6);


	/* Send packet */
	if (sendto(sockfd, sendDataBuffer, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
	    printf("Send failed\n");
		// for (int j = 0; j< tx_len;j++)
		// 	printf("%02x ",sendDataBuffer[j]);
		// printf("\n");
	return 0;
}