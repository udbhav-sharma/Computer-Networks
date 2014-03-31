/*
Child interacting through TCP
*/
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

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
	int portno,n;
	char buf[256];
	struct sockaddr_in serv_addr;
	struct hostent *server;

	sfd=socket(AF_INET,SOCK_STREAM,0);
	if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }

    portno=8016;
    serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(portno);
    if((n=connect(sfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)))<0) 
    {
        perror("ERROR connecting");
        exit(0);
    }
    
    while(1)
    {
        strcpy(buf,"");
        scanf("%s",buf);
        send(sfd,buf,strlen(buf)+1,0);
        recv(sfd,buf,255,0);
        write(1,buf,strlen(buf));
        write(1,"\n",1);
    }
    close(sfd);
    exit(1);
}