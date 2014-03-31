#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc,char* argv[])
{
	int s;
	struct sockaddr_in saddr;
	char packet[1000];
	int on=1;
	if ((s = socket(AF_INET, SOCK_RAW, atoi(argv[1]))) < 0) {
		perror("error:");
		exit(EXIT_FAILURE);
	}

	socklen_t *len = (socklen_t *)sizeof(saddr);
	int fromlen = sizeof(saddr);int len1=sizeof(on);
	if(setsockopt(s,IPPROTO_IP,IP_HDRINCL,&on,sizeof(on))<0)
	{
		perror("setsockopt():\n");
		exit(1);
	}

	while(1) 
	{
		if (recvfrom(s, (char *)&packet, sizeof(packet), 0,
			(struct sockaddr *)&saddr, &fromlen) < 0)
			perror("packet receive error:");
		struct iphdr *ip=(struct iphdr *)packet; 
		unsigned char b;
		b = (ip->daddr >> 24) & 0xFF;
		if(atoi(argv[1])==ip->protocol)
			printf("accepted\n");
		else printf("not accepted\\n");
		int i = 0;
		
		sleep(2);

		printf("%d,%d,%d,%d",ip->ihl,ip->version,ip->tos,ip->check);
		
		char *temp_ip;
    	temp_ip=inet_ntoa(ip->saddr.sin_addr);
    	printf("%s\n",temp_ip);

		temp_ip=inet_ntoa(ip->daddr.sin_addr);
		printf("%s\n",temp_ip);

		printf("%s\n",packet);
		printf("\n");
	}
	exit(EXIT_SUCCESS);
}
