#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>

int sfd;

void closesfd(int sig)
{
    close(sfd);
    write(1,"Byee\n",5);
    exit(0);
}

main()
{
    (void )signal(SIGINT,closesfd);
	sfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(sfd<0)
	{
		perror("Error in creating socket DGRAM");	
	}
	struct sockaddr_in serv_addr,cli_addr;
	int cli_len=sizeof(cli_addr);
	int portno=9001;
	int portno2=8001;

	serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    cli_addr.sin_port = htons(portno2);

    if (bind(sfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(0);
    }
    char buf[255];
    write(1,"wating for recvfrom\n",strlen("wating for recvfrom\n"));
    int k=recvfrom(sfd,buf,255,0,(struct sockaddr *) &cli_addr,&cli_len);
    if(k<0)
    {
        perror("Error in recv from\n");
        exit(0);
    }
    write(1,buf,strlen(buf)+1);
    sleep(2);
    close(sfd);
    exit(1);
}