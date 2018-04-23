/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 */

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
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


int main(int argc, char *argv[])
{
	char sender[INET6_ADDRSTRLEN];
	int sockfd, ret, i;
	int sockopt;
	ssize_t numbytes;
	struct ifreq ifopts;	/* set promiscuous mode */
	struct sockaddr_storage their_addr;
	uint8_t buf[BUFFER_SIZE];
	char interfaceName[IFNAMSIZ];
	char my_dest_name[DEST_NAME_SIZE];
	u_char MACAddr[6];
	char packet_dest_name[DEST_NAME_SIZE];
	char packet_source_name[SOURCE_NAME_SIZE];
	char* message;
	
	/* Get interface name */
	if (argc > 2){
		strcpy(interfaceName, argv[1]);
		strncpy(my_dest_name, argv[2], 10);
	}else{
		fprintf(stderr,"Invalid arguments\n");
		return 1;
	}

	/* Header structures */
	struct ether_header *eh = (struct ether_header *) buf;
	struct iphdr *iph = (struct iphdr *) (buf + sizeof(struct ether_header));
	struct udphdr *udph = (struct udphdr *) (buf + sizeof(struct iphdr) + sizeof(struct ether_header));

	//memset(&if_ip, 0, sizeof(struct ifreq));

	/* Open PF_PACKET socket, listening for EtherType ETHER_TYPE */
	if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE))) == -1) {
		perror("listener: socket");	
		exit(EXIT_FAILURE);
	}

	/* Set interface to promiscuous mode - do we need to do this every time? */
	strncpy(ifopts.ifr_name, interfaceName, IFNAMSIZ-1);
	ioctl(sockfd, SIOCGIFFLAGS, &ifopts);
	ifopts.ifr_flags |= IFF_PROMISC;
	ioctl(sockfd, SIOCSIFFLAGS, &ifopts);
	if (ioctl(sockfd, SIOCGIFHWADDR, &ifopts) < 0){
	    perror("SIOCGIFHWADDR");
		close(sockfd);
		exit(EXIT_FAILURE);}
	memcpy(MACAddr,ifopts.ifr_hwaddr.sa_data,6);

	/* Allow the socket to be reused - incase connection is closed prematurely */
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1) {
		perror("setsockopt");
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	/* Bind to device */
	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, interfaceName, IFNAMSIZ-1) == -1)	{
		perror("SO_BINDTODEVICE");
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	// /* Get the MAC address of the interface to send on */
	// memset(&if_mac, 0, sizeof(struct ifreq));
	// strncpy(if_mac.ifr_name, interfaceName, IFNAMSIZ-1);
	// if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
	//     perror("SIOCGIFHWADDR");
	printf("My MAC: %x:%x:%x:%x:%x:%x\n",
							MACAddr[0],
							MACAddr[1],
							MACAddr[2],
							MACAddr[3],
							MACAddr[4],
							MACAddr[5]);
	while(1){
		printf("Waiting for packet...\n");
		numbytes = recvfrom(sockfd, buf, BUFFER_SIZE, 0, NULL, NULL);
		printf("Got packet %lu bytes\n", numbytes);
		if (eh->ether_dhost[0] == MACAddr[0] &&
			eh->ether_dhost[1] == MACAddr[1] &&
			eh->ether_dhost[2] == MACAddr[2] &&
			eh->ether_dhost[3] == MACAddr[3] &&
			eh->ether_dhost[4] == MACAddr[4] &&
			eh->ether_dhost[5] == MACAddr[5]) {
			printf("Correct destination MAC address\n");
		} else {
			printf("Wrong destination MAC: %x:%x:%x:%x:%x:%x\n",
							eh->ether_dhost[0],
							eh->ether_dhost[1],
							eh->ether_dhost[2],
							eh->ether_dhost[3],
							eh->ether_dhost[4],
							eh->ether_dhost[5]);
		}
		char* ptr = buf+14;
		strncpy(packet_dest_name,ptr,DEST_NAME_SIZE);
		ptr+=DEST_NAME_SIZE;
		strncpy(packet_source_name,ptr,SOURCE_NAME_SIZE);
		ptr+=SOURCE_NAME_SIZE;
		int sizeMessage = numbytes - 14 - DEST_NAME_SIZE - SOURCE_NAME_SIZE;
		message = malloc((sizeMessage+1)*sizeof(char));
		strncpy(message,ptr,sizeMessage);
		if (strncmp(my_dest_name,packet_dest_name,strlen(my_dest_name))==0){
			printf("It's mine!\n");
		}
		printf("Source: %s\nDestination: %s\nMessage: %s\n\n",packet_source_name,packet_dest_name,message);
		// 	/* Print packet */
		// printf("\tData:");
		// for (i=0; i<numbytes; i++) printf("%02x:", buf[i]);
		// printf("\n");

	}
	

	// /* Check the packet is for me */
	

	// /* Get source IP */
	// ((struct sockaddr_in *)&their_addr)->sin_addr.s_addr = iph->saddr;
	// inet_ntop(AF_INET, &((struct sockaddr_in*)&their_addr)->sin_addr, sender, sizeof sender);

	// /* Look up my device IP addr if possible */
	// strncpy(if_ip.ifr_name, interfaceName, IFNAMSIZ-1);
	// if (ioctl(sockfd, SIOCGIFADDR, &if_ip) >= 0) { /* if we can't check then don't */
	// 	printf("Source IP: %s\n My IP: %s\n", sender, 
	// 			inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr));
	// 	/* ignore if I sent it */
	// 	if (strcmp(sender, inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr)) == 0)	{
	// 		printf("but I sent it :(\n");
	// 		ret = -1;
	// 		goto done;
	// 	}
	// }

	// /* UDP payload length */
	// ret = ntohs(udph->len) - sizeof(struct udphdr);



	close(sockfd);
	return 0;
}