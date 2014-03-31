#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

main()
{
	int sfd,portno,n;
	char buf[256];
	struct sockaddr_in serv_addr;
	struct hostent *server;

	sfd=socket(AF_INET,SOCK_STREAM,0);
	if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }

    portno=11111;
    serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(portno);
    if((n=connect(sfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)))<0) 
    {
        perror("ERROR connecting");
        exit(0);
    }
    printf("Connection done.\n");
    fflush(stdout);

    recv(sfd,buf,255,0);
    printf("Received msg: %s\n",buf);
    fflush(stdout);

    recv(sfd,buf,255,0);
    printf("Received msg: %s\n",buf);
    fflush(stdout);

    close(sfd);
    exit(1);
}