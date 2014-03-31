#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

main()
{
    int sfd,sfd2,nsfd,nsfd2,portno,clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr, new_serv_addr;

    sfd=socket(AF_INET,SOCK_STREAM,0);
    sfd2=socket(AF_INET,SOCK_STREAM,0);
    if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }
    portno=12112;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    portno=11112;
    new_serv_addr.sin_family = AF_INET;
    new_serv_addr.sin_addr.s_addr = INADDR_ANY;
    new_serv_addr.sin_port = htons(portno);

    if(bind(sfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(0);
    }
    
    if(bind(sfd2,(struct sockaddr *) &new_serv_addr, sizeof(new_serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(0);
    }

    listen(sfd,5);
    listen(sfd2,5);
    clilen=sizeof(cli_addr);
    
    while(1)
    {
        char buf[255];
        write(1,"Waiting for accept\n",20);

        nsfd=accept(sfd,(struct sockaddr *)&cli_addr,&clilen);
        
        write(1,"Waiting for read\n",18);

        read(nsfd,buf,255);
        write(1,buf,strlen(buf)+1);
        write(nsfd,"Yes",4);
        int pid=fork();
        if(pid>0)
        {
            close(nsfd);
            continue;
        }
        else if(pid==0)
        {
            close(nsfd);
            struct sockaddr_in cli_addr2;
            int clilen2=sizeof(cli_addr2);
            nsfd2=accept(sfd2,(struct sockaddr *)&cli_addr2,&clilen2);
            break;
        }
    }
    exit(1);
}