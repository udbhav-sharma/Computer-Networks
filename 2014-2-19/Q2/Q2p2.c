#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

int sfd;
struct sockaddr_in serv_addr,cli_addr;
int cli_len=sizeof(cli_addr);

void closesfd(int sig)
{
    close(sfd);
    write(1,"Byee\n",5);
    exit(0);
}

void *reading(void *arg)
{
    char buf[255];
    while(1)
    {
        write(1,"Thread waiting for recvfrom\n",strlen("Thread waiting for recvfrom\n"));
        int k=recvfrom(sfd,buf,255,0,(struct sockaddr *) &cli_addr,&cli_len);
        if(k<0)
        {
            perror("Error in recv from\n");
            exit(0);
        }
        write(1,"Thread read:\t",strlen("Thread read:\t"));
        //buf[k]='\0';
        write(1,buf,strlen(buf)+1);
    }
}

void *writing(void *arg)
{
    char buf[255];
    while(1)
    {
        write(1,"Thread Waiting for your input\n",strlen("Thread waiting for your input\n"));
        read(1,buf,255);
        int k=sendto(sfd,buf,strlen(buf)+1,0,(struct sockaddr *)&cli_addr,cli_len);
        if(k<0)
        {
            perror("Error in sendto\n");
            exit(0);
        }
    }
}

main()
{
    (void )signal(SIGINT,closesfd);
    sfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(sfd<0)
    {
        perror("Error in creating socket DGRAM");   
    }

    int portno=8001;
    int portno2=9001;

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

    pthread_t read_thread,write_thread;
    pthread_create(&read_thread,NULL,reading,NULL);
    pthread_create(&write_thread,NULL,writing,NULL);

    pthread_join(read_thread,NULL);
    pthread_join(write_thread,NULL);

    close(sfd);
    exit(1);
}