#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/un.h>
#include <sys/msg.h>
#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define numberofconnections 10
#define numberofinterviewers 3

int sfd,portno;

int usfd;
int nusfd[numberofinterviewers];

sem_t mutex_choose_client;

struct client_details
{
    char IP[255];
    int portno;
    int nsfd;
    int free;
    int last_interview;
    int interview_count;
};

struct client_details clients[numberofconnections];
int client_size=0;

void byee()
{
    int i;
    for(i=0;i<client_size;i++)
        close(clients[i].nsfd);
    for(i=0;i<numberofinterviewers;i++)
        close(nusfd[i]);
    close(sfd);
    close(usfd);

    printf("Bye\n");
    fflush(stdout);
}

void closesfds(int sig)
{
    byee();
    exit(1);
}

void *accept_connections()
{
    sfd=socket(AF_INET,SOCK_STREAM,0);
    if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }

    struct sockaddr_in serv_addr;

    portno =8888;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(0);
    }
    
    listen(sfd,numberofconnections);

    struct sockaddr_in cli_addr;
    int clilen;

    while(1)
    {
        clilen=sizeof(cli_addr);
        
        clients[client_size].nsfd=accept(sfd,(struct sockaddr *)&cli_addr,&clilen);

        char *temp_ip;
        temp_ip=inet_ntoa(cli_addr.sin_addr);
        
        strcpy(clients[client_size].IP,temp_ip);
        clients[client_size].portno=ntohs(cli_addr.sin_port);
        clients[client_size].interview_count=0;
        clients[client_size].last_interview=-1;
        clients[client_size].free=1;

        printf("Connection created.\n");
        printf("Aptitude Test started.\n");
        fflush(stdout);
        send(clients[client_size].nsfd,"Aptitude test going on",strlen("Aptitude test going on"),0);
        printf("Aptitude test completed.\n");
        fflush(stdout);

        sem_wait(&mutex_choose_client);
        client_size++;
        sem_post(&mutex_choose_client);
    }
    pthread_exit(NULL);
}

void free_client(int i)
{
    int j;
    for(j=0;j<client_size;j++)
        if(clients[j].last_interview==i)
            clients[j].free=1;
}

int choose_client(int i)
{
    int j;
    for(j=0;j<client_size;j++)
        if((clients[j].interview_count<3 && clients[j].free==1 && 
            ( clients[j].last_interview==(i+numberofinterviewers-1)%numberofinterviewers)
            ||clients[j].last_interview==-1))
        {
            clients[j].interview_count++;
            clients[j].last_interview=i;
            clients[j].free=0;
            return clients[j].nsfd;
        }
    return -1;
}

void *deal_with_interviewers(void* arg)
{
    int nsfd;
    int i=*((int *)arg);
    char buffer[255];
    while(1)
    {
        recv(nusfd[i],buffer,255,0);
        printf("Interviewer %d is free.\n",i);
        fflush(stdout);

        sem_wait(&mutex_choose_client);
        free_client(i);
        sem_post(&mutex_choose_client);
        
        printf("Interviewer %d waiting for client\n",i);
        fflush(stdout);

        do
        {
            sem_wait(&mutex_choose_client);
            nsfd=choose_client(i);
            sem_post(&mutex_choose_client);
        }
        while(nsfd==-1);

        printf("Client Selected for Interviewer %d\n",i);
        fflush(stdout);

        char buf[1];
        struct iovec iov;
        struct msghdr msg;
        struct cmsghdr *cmsg;
        char cms[CMSG_SPACE(sizeof(int))];

        buf[0] = 0;
        iov.iov_base = buf;
        iov.iov_len = 1;

        memset(&msg, 0, sizeof msg);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = (caddr_t)cms;
        msg.msg_controllen = CMSG_LEN(sizeof(int));

        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_len = CMSG_LEN(sizeof(int));
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        memmove(CMSG_DATA(cmsg), &nsfd, sizeof(int));
        sendmsg(nusfd[i], &msg, 0);
    }
    pthread_exit(NULL);
}

void *create_socket_for_interviewers()
{
    usfd=socket(AF_UNIX,SOCK_STREAM,0);
    if (usfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }

    struct sockaddr_un serv_addr;
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path,"/tmp/temp_AT");
    unlink("/tmp/temp_AT");

    if (bind(usfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(0);
    }
    
    listen(usfd,numberofinterviewers);

    int i;
    
    struct sockaddr_un cli_addr;
    int clilen=sizeof(cli_addr);
    int error;
    pthread_t interviewers_thread[numberofinterviewers]; 

    for(i=0;i<numberofinterviewers;i++)
    {
        nusfd[i]=accept(usfd,(struct sockaddr *)&cli_addr,&clilen);
        if(nusfd[i]<0)
        {
            perror("Error on accept");
            exit(0);
        }

        int* p;
        p=malloc(sizeof(int));
        *p=i;
        error=pthread_create(&interviewers_thread[i],NULL,deal_with_interviewers,p);
        if(error<0)
        {
            perror("ERROR in read_thread:");
            exit(0);
        }
    }
    pthread_exit(NULL);
}

main()
{
    (void) signal(SIGINT,closesfds);

    sem_init(&mutex_choose_client,0,1);

    pthread_t connection_thread,read_thread;
    int error=pthread_create(&connection_thread,NULL,accept_connections,NULL);
    if(error<0)
    {
        perror("ERROR in connection_thread:");
        exit(0);
    }
    error=pthread_create(&read_thread,NULL,create_socket_for_interviewers,NULL);
    if(error<0)
    {
        perror("ERROR in read_thread:");
        exit(0);
    }
    pthread_join( connection_thread, NULL);
    pthread_join( read_thread, NULL);
    
    byee();
	exit(1);
}