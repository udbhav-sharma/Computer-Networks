#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h> 
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>

int sfd;

void closesfd(int sig)
{
    close(sfd);
    write(1,"Hv a Wonderful Day\n",strlen("Hv a Wonderfull Day\n"));
    exit(1);
}

main()
{
    (void)signal(SIGINT,closesfd);

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

    portno=9006;
    serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(portno);
    if((n=connect(sfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)))<0) 
    {
        perror("ERROR connecting");
        exit(0);
    }
    
    char user_message[255];
    char recv_message[255];

    send(sfd,"p3\0",3,0);
    scanf("%d",&n);
    send(sfd,"#List\0",strlen("#List")+1,0);

    write(1,"Adjacenecy List\n",strlen("Adjacenecy List\n"));
    while(1)
    {
        scanf("%s",user_message);
        send(sfd,user_message,strlen(user_message)+1,0);
        sleep(2);
        if(strcmp(user_message,"-1")==0)
            break;
    }

    fd_set fdarray,fdarray2;
    FD_ZERO(&fdarray);
    FD_ZERO(&fdarray2);

    FD_SET(0,&fdarray2);
    FD_SET(sfd,&fdarray2);

    while(1)
    {
        fdarray=fdarray2;

        struct timeval tv;
        tv.tv_usec=0;
        tv.tv_sec=2;

        int ret=select(sfd+1, &fdarray, NULL, NULL,&tv);

        if(ret>0 && FD_ISSET(0,&fdarray))
        {
            scanf("%s",user_message);
            send(sfd,user_message,strlen(user_message)+1,0);
            write(1,"Message Sent\n",strlen("Message Sent\n"));
        }

        else if(ret>0)
        {
            recv(sfd,recv_message,255,0);
            write(1,"Received Message:\t",strlen("Received Message:\t"));
            write(1,recv_message,strlen(recv_message)+1);
            write(1,"\n",1);
        }
    }
    close(sfd);
    exit(1);
}
