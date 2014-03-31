#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

main()
{
	int sfd,nsfd,portno,clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;

	sfd=socket(AF_INET,SOCK_STREAM,0);
	if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }
    portno =8888;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(0);
    }
    
    listen(sfd,5);
    clilen = sizeof(cli_addr);
    
    nsfd = accept(sfd,(struct sockaddr *)&cli_addr,&clilen);
    if(nsfd < 0) 
    {
        perror("ERROR on accept");
        exit(0);
    }

    int n=recv(nsfd,buffer,255,0);
    if(n<0)
    {
        perror("ERROR reading from socket");
        exit(0);
    }
    printf("Here is the message: %s\n",buffer);
    fflush(stdout);
    
    n=send(nsfd,"I got your message",18,0);
    if(n<0)
    {
        perror("ERROR writing to socket");
        exit(0);
    }
    close(nsfd);
    close(sfd);
	exit(1);
}