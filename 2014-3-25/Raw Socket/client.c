#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>

void print_ip(int ip)
{
    unsigned char bytes[4];
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;	
    printf("  %d.%d.%d.%d,", bytes[0], bytes[1], bytes[2], bytes[3]);        
}

int main(int argc,char* argv[])
{
	int s;
	struct sockaddr_in saddr;
	char packet[1000];
	const int on=1;

	if ((s = socket(AF_INET, SOCK_RAW, atoi(argv[1]))) < 0) 
	{
		perror("error:");
		exit(EXIT_FAILURE);
	}

	int fromlen = sizeof(saddr);

	if(setsockopt(s,IPPROTO_IP,IP_HDRINCL,&on,sizeof(on))<0)
	{
		perror("setsockopt():\n");
		exit(1);
	}

	while(1) 
	{
		if (recvfrom(s, (char *)&packet, sizeof(packet), 0,(struct sockaddr *)&saddr, &fromlen) < 0)
			perror("packet receive error:");

		struct iphdr *ip=(struct iphdr *)packet; 

		if(atoi(argv[1])==ip->protocol)
			printf("accepted\n");
		else 
			printf("not accepted\\n");

		printf("%d,%d,%d,%d",ip->ihl,ip->version,ip->tos,ip->check);
		print_ip(ip->saddr);
		print_ip(ip->daddr);
		char *k=packet;
		k+=20;
		printf("%s\n",k);
	}
	
	exit(EXIT_SUCCESS);
}
