#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>

int sfd[3];
int mall_nsfd[3];
int mall_sfd;

struct clients_details
{
    char IP[255];
    char ticket[255];
};

struct clients_details client_arrived[100];
int client_arrived_size=0;

void show_all_clients()
{
    int i;
    printf("IP\t\tTicket\n");
    for(i=0;i<client_arrived_size;i++)
        printf("%s\t%s\n",client_arrived[i].IP,client_arrived[i].ticket);
}

void closeallsfds()
{
    int i;
    for(i=0;i<3;i++)
        close(sfd[i]);
    for(i=0;i<3;i++)
        close(mall_nsfd[i]);
    close(mall_sfd);
}

void closesfd(int sig)
{
    closeallsfds();
    write(1,"Bye\n",strlen("Bye\n"));
    exit(1);
}

int check_for_validity(char client_ip[255],char client_ticket[255])
{
    printf("client_ip:%s\tClient ticket:%s\n",client_ip,client_ticket);
    int i;
    for(i=0;i<client_arrived_size;i++)
        if(strcmp(client_arrived[i].IP,client_ip)==0 && 
            strcmp(client_arrived[i].ticket,client_ticket)==0)
            return 1;
    return 0;
}

struct msg{
    char client_ip[255];
    char client_ticket[255];
};

void* handle_mall_connection(void* arg)
{
    fd_set fdarray,fdarray2;
    FD_ZERO(&fdarray);
    FD_ZERO(&fdarray2);

    int max_fd=0;
    int i;
    for(i=0;i<3;i++)
    {
        struct sockaddr_in cli_addr;
        int clilen=sizeof(cli_addr);
        mall_nsfd[i]=accept(mall_sfd,(struct sockaddr *)&cli_addr,&clilen);
        FD_SET(mall_nsfd[i],&fdarray);
        FD_SET(mall_nsfd[i],&fdarray2);
        if(max_fd<mall_nsfd[i])
            max_fd=mall_nsfd[i];
    }

    while(1)
    {
        fdarray=fdarray2;
        struct timeval tv;
        tv.tv_usec=0;
        tv.tv_sec=2;

        int ret=select(max_fd+1,&fdarray,NULL,NULL,&tv);

        if(ret>0)
        {
            for(i=0;i<3;i++)
            {
                if(FD_ISSET(mall_nsfd[i],&fdarray))
                {
                    struct msg t;
                    char status[255];
                    recv(mall_nsfd[i],&t,sizeof(t),0);
                    if(check_for_validity(t.client_ip,t.client_ticket))
                        strcpy(status,"1");
                    else
                        strcpy(status,"0");
                    send(mall_nsfd[i],status,strlen(status)+1,0);
                }
            }
        }
    }
}

main()
{
    (void)signal(SIGINT,closesfd);

	int portno,clilen;
	char buffer[256];
	struct sockaddr_in serv_addr;

    mall_sfd=socket(AF_INET,SOCK_STREAM,0);

    portno=20000;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port=htons(portno);

    if(bind(mall_sfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding in mall socket");
        exit(0);
    }

    listen(mall_sfd,3);

    pthread_t mall_servers;
    pthread_create(&mall_servers,NULL,handle_mall_connection,NULL);

    //Creating 3 gates
    int i;
    for(i=0;i<3;i++)
    {
	    sfd[i]=socket(AF_INET,SOCK_STREAM,0);
        if(sfd[i]<0) 
        {
            perror("ERROR in opening tcp socket");
            exit(0);
        }
    }

    portno=8888;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    for(i=0;i<3;i++)
    {
        serv_addr.sin_port=htons(portno+i);
        if (bind(sfd[i],(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        {
           perror("ERROR on binding in tcp socket");
           exit(0);
        }
       listen(sfd[i],5);
    }
    
    //Listening on 3 Gates
    fd_set fdarray,fdarray2;
    FD_ZERO(&fdarray);
    FD_ZERO(&fdarray2);

    int max_fd=0;

    for(i=0;i<3;i++)
    {
        FD_SET(sfd[i],&fdarray);
        FD_SET(sfd[i],&fdarray2);
        if(max_fd<sfd[i])
            max_fd=sfd[i];
    }

    while(1)
    {
        fdarray=fdarray2;
        struct timeval tv;
        tv.tv_usec=0;
        tv.tv_sec=2;

        int ret=select(max_fd+1,&fdarray,NULL,NULL,&tv);

        if(ret>0)
        {
            for(i=0;i<3;i++)
            {
                if(FD_ISSET(sfd[i],&fdarray))
                {
                    struct sockaddr_in cli_addr;
                    clilen = sizeof(cli_addr);
                    int nsfd=accept(sfd[i],(struct sockaddr *)&cli_addr,&clilen);
                    if(nsfd < 0) 
                    {
                        perror("ERROR on accept");
                        exit(0);
                    }
                    char buffer[255];
                    char ticket[255];
                    write(1,"Waiting for receive\n",strlen("Waiting for receive\n"));
                    recv(nsfd,buffer,255,0);
                    if(strcmp(buffer,"C")==0 || strcmp(buffer,"-1")==0)
                    {
                        send(nsfd,"127.0.0.1\0",strlen("127.0.0.1")+1,0);
                        sleep(2);
                        send(nsfd,"9000\0",strlen("9000")+1,0);
                    }
                    else if(strcmp(buffer,"S")==0) 
                    {
                        send(nsfd,"127.0.0.1\0",strlen("127.0.0.1")+1,0);
                        sleep(2);
                        send(nsfd,"10000\0",strlen("10000")+1,0);
                    }
                    else if(strcmp(buffer,"E")==0) 
                    {
                        send(nsfd,"127.0.0.1\0",strlen("127.0.0.1")+1,0);
                        sleep(2);
                        send(nsfd,"11000\0",strlen("11000")+1,0);
                    }
                    write(1,"Enter ticket\n",strlen("Enter ticket\n"));
                    scanf("%s",ticket);
                    send(nsfd,ticket,strlen(ticket)+1,0);
                    write(1,"Send Info\n",strlen("Send Info\n"));

                    char *temp_ip;
                    temp_ip=inet_ntoa(cli_addr.sin_addr);
                    strcpy(client_arrived[client_arrived_size].ticket,ticket);
                    strcpy(client_arrived[client_arrived_size].IP,temp_ip);
                    client_arrived_size++;

                    show_all_clients();
                }
            }
        }
    }

    closeallsfds();
	exit(1);
}