#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define num 5

int sfd;
int nsfd[num];
int nsfd_size=0;

fd_set r_fds,r_fds2;
int max_fd=-1;

struct token
{
    char receiver_IP[255];
    int receiver_portno;
    char sender_IP[255];
    int sender_portno;
    char msg[255];
    int writable;
};

void* accept_connection(void* arg)
{
    struct sockaddr_in client;
    int c=sizeof(client);

    FD_ZERO(&r_fds2);
    FD_ZERO(&r_fds);

    while(1)
    {
        nsfd[nsfd_size]=accept(sfd,(struct sockaddr *)&client,(socklen_t*)&c);
        FD_SET(nsfd[nsfd_size],&r_fds2);
        if(max_fd<nsfd[nsfd_size])
            max_fd=nsfd[nsfd_size];
        nsfd_size++;
    }
}

void closesfd(int sig)
{
    int i;
    for(i=0;i<num;i++)
        close(nsfd[i]);
    close(sfd);
}

int main()
{
    (void)signal(SIGINT,closesfd);
    int client_sock ;
    struct sockaddr_in server , client;
     
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    int port_num = 9089;
    server.sin_port = htons( port_num);
   
    sfd=socket(AF_INET,SOCK_STREAM,0);
    int n;

    if((n=bind(sfd,(struct sockaddr *)&server ,sizeof(server)))<0)
    {
        perror("Error in binding\n");
        exit(0);
    }
    listen(sfd,3);

    int c=sizeof(client);
    
    pthread_t thread1;
    pthread_create(&thread1,NULL,accept_connection,NULL);

    struct timeval tv;
    tv.tv_sec=2;
    tv.tv_usec=0;

    struct token t;
    int i;

    printf("For Start enter number\n");
    scanf("%d",&i);

    t.writable=1;
    int cycle=0;
    send(nsfd[cycle++],&t,sizeof(t),0);
    
    while(1)
    {
        r_fds=r_fds2;
        int ret=select(max_fd+1,&r_fds,NULL,NULL,&tv);
        if(ret>0)
        {
            for(i=0;i<nsfd_size;i++)
                if(FD_ISSET(nsfd[i],&r_fds))
                {
                    recv(nsfd[i],&t,sizeof(t),0);

                    printf("Token contents are:\nReceiver IP: %s\tReceiver Portno: %d\nSender IP: %s\tSender Portno: %d\nMsg: %s\n",
                        t.receiver_IP,t.receiver_portno,t.sender_IP,t.sender_portno,t.msg);
                    fflush(stdout);

                    send(nsfd[cycle++],&t,sizeof(t),0);
                    if(cycle==nsfd_size)
                        cycle=0;
                    break;
                }
        }
    }

    for(i=0;i<nsfd_size;i++)
        close(nsfd[i]);
    close(sfd);
    exit(1);
}
