#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/in_systm.h>

int main(int argc,char** argv)
{
	int sd;
	const int on =1;
	struct sockaddr_in daddr;
	char packet[1000];
	struct iphdr *ip = (struct iphdr *)packet;  

	if ((sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) 
	{
		perror("error:");
		exit(EXIT_FAILURE);
	}


	daddr.sin_family = AF_INET;
	daddr.sin_port = 0; 
	daddr.sin_addr.s_addr=inet_addr("127.0.0.1");

	memset(daddr.sin_zero, 0, sizeof(daddr.sin_zero));

	char *k=packet;
	k+=20;
	strcpy(k,"Udbhav Sharma");
	
	packet[999]=0;
	ip->ihl = 5;
	ip->version = 4;
	ip->tos = 0;
	ip->tot_len = htons(40);	
	ip->frag_off = 0;		
	ip->ttl = 64;			
	ip->protocol = 2;	
	ip->check = 0;		
	ip->saddr = inet_addr("1.2.3.4");
	ip->daddr = inet_addr("127.0.0.1");

	if(setsockopt(sd,IPPROTO_IP,IP_HDRINCL,&on,sizeof(on))<0)
	{
		perror("setsockopt():\n");
		exit(1);
	}

	int count=0;
	while(count<10)
	{
		int n;
		printf("enter protocolno: \n");
		scanf("%d",&n);
		ip->protocol = n;	
		if (sendto(sd, (char *)packet, sizeof(packet), 0, 
			(struct sockaddr *)&daddr, (socklen_t)sizeof(daddr)) < 0)
			perror("packet send error:");
		count++;
	}
	exit(EXIT_SUCCESS);
}

