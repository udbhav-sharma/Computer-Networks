#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define numberofmalls 3
#define mall_starting_portno 11000

int sfd,nsfd,mall_sfd,mall_nsfd[3];
int ticket_no=100;

struct mall_addr
{
    char IP[256];
    int portno;
};

struct mall_addr *mall;

struct client_came_info_structure
{
    char IP[255];
    int portno;
    int ticket_no;
};

struct client_came_info_structure *client_came_info;
int client_came_info_size=0;

void byee()
{
    close(mall_sfd);
    int i;
    for(i=0;i<numberofmalls;i++)
        close(mall_nsfd[3]);
    close(nsfd);
    close(sfd);
    printf("Byee\n");
    fflush(stdout);
}

void closesfd(int sig)
{
    byee();
    exit(1);
}

int check_ticket(struct client_came_info_structure mall_client)
{
    int i;
    for(i=0;i<client_came_info_size;i++)
        if(strcmp(mall_client.IP,client_came_info[i].IP)==0 && 
            mall_client.portno==client_came_info[i].portno && 
            mall_client.ticket_no==client_came_info[i].ticket_no)
        return 1;
    return 0;
}

void *handle_malls()
{
    int portno,clilen;
    struct sockaddr_in serv_addr, cli_addr;

    mall_sfd=socket(AF_INET,SOCK_STREAM,0);
    if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }
    portno=8888;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(mall_sfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(0);
    }
    
    listen(mall_sfd,3);
    clilen=sizeof(cli_addr);

    fd_set r_fd,r_fd2;
    int max_fd;

    FD_ZERO(&r_fd);
    FD_ZERO(&r_fd2);

    int i;
    for(i=0;i<numberofmalls;i++)
    {
        mall_nsfd[i]=accept(mall_sfd,(struct sockaddr *)&cli_addr,&clilen);
        FD_SET(mall_nsfd[i],&r_fd2);
        if(max_fd<mall_nsfd[i])
            max_fd=mall_nsfd[i];
    }

    struct timeval tv;
    tv.tv_usec=0;
    tv.tv_sec=2;

    printf("All malls got connected\n");
    fflush(stdout);

    while(1)
    {
        r_fd=r_fd2;
        int ret=select(max_fd+1,&r_fd,NULL,NULL,&tv);
        if(ret>0)
        {
            for(i=0;i<numberofmalls;i++)
            {
                if(FD_ISSET(mall_nsfd[i],&r_fd))
                {
                    struct client_came_info_structure mall_client;
                    recv(mall_nsfd[i],&mall_client,sizeof(struct client_came_info_structure),0);
                    
                    int status=check_ticket(mall_client);

                    send(mall_nsfd[i],&status,sizeof(status),0);

                    printf("Ticket Status %d sent to mall %d\n",status,i);
                    fflush(stdout);

                }
            }
        }
    }
}

main()
{
    (void) signal(SIGINT,closesfd);

    pthread_t thread;
    pthread_create(&thread,NULL,handle_malls,NULL);

    //Forming mall addresses
    mall=malloc(sizeof(struct mall_addr)*numberofmalls);
    client_came_info=malloc(sizeof(struct client_came_info_structure)*100);

    int i;
    for(i=0;i<numberofmalls;i++)
    {
        strcpy(mall[i].IP,"127.0.0.1");
        mall[i].portno=mall_starting_portno+i;
    }

	int portno,clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;

    //Creating socket for TCP connection for clients
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }
    portno=8889;
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
    
    while(1)
    {
        nsfd = accept(sfd,(struct sockaddr *)&cli_addr,&clilen);
        if(nsfd < 0) 
        {
            perror("ERROR on accept");
            exit(0);
        }

        char *temp_ip;
        temp_ip=inet_ntoa(cli_addr.sin_addr);

        strcpy(client_came_info[client_came_info_size].IP,temp_ip);
        client_came_info[client_came_info_size].portno=ntohs(cli_addr.sin_port);
        client_came_info[client_came_info_size].ticket_no=ticket_no++;
        
        printf("Client connected with\nIp: %s Portno: %d ticket_no: %d\n",
            client_came_info[client_came_info_size].IP,
            client_came_info[client_came_info_size].portno,
            client_came_info[client_came_info_size].ticket_no);

        fflush(stdout);

        int n=recv(nsfd,buffer,255,0);
        buffer[n]='\0';

        printf("Client wants to connect with: %s\n",buffer);
        fflush(stdout);

        if(strcmp(buffer,"A")==0)
            i=0;
        else if(strcmp(buffer,"B")==0)
            i=1;
        else
            i=2;

        struct client_came_info_structure mall_info;

        strcpy(mall_info.IP,mall[i].IP);
        mall_info.portno=mall[i].portno;
        mall_info.ticket_no=client_came_info[client_came_info_size].ticket_no;
        
        client_came_info_size++;

        send(nsfd,&mall_info,sizeof(mall_info),0);

        printf("Send IP and Portno\n");
        fflush(stdout);

        close(nsfd);
    }
    byee();
	exit(1);
}