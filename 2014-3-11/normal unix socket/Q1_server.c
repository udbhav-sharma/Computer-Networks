#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

main()
{
    int sfd,nsfd,clilen;
    char buffer[256];
    struct sockaddr_un serv_addr, cli_addr;

    sfd=socket(AF_UNIX,SOCK_STREAM,0);
    if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }

    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path,"/tmp/temp.txt");
    unlink("/tmp/temp.txt");

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

    recv(nsfd,buffer,255,0);
    printf("Client message: %s\n",buffer);
    fflush(stdout);
    send(nsfd,"I got your message",strlen("I got your message"),0);
    
    close(nsfd);
    close(sfd);

    exit(1);
}