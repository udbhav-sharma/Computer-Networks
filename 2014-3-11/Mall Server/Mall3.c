#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/un.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define my_portno 11002
#define max_connection_limit 5

int sfd,ticket_counter_sfd;
int usfd,nusfd,usfd2;
int no_of_clients=0;

sem_t check_for_client_space;

void byee()
{
    close(sfd);
    close(nusfd);
    close(usfd);
    close(usfd2);
    close(ticket_counter_sfd);
    printf("Byee\n");
    fflush(stdout);
}

void closesfd(int sig)
{
    byee();
    exit(1);
}

struct client_info_structure
{
    char IP[255];
    int portno;
    int ticket_no;
};

void *handling_client(void *arg)
{
    int nsfd=*((int *)arg);

    struct sockaddr_in cli_addr;
    int clilen=sizeof(cli_addr);

    getsockname(nsfd,(struct sockaddr *)&cli_addr,&clilen);
    
    char *temp_ip;
    temp_ip=inet_ntoa(cli_addr.sin_addr);

    struct client_info_structure client_info;

    strcpy(client_info.IP,temp_ip);
    client_info.portno=ntohs(cli_addr.sin_port);

    printf("Client connected with\nIp: %s and Portno: %d\n",client_info.IP,client_info.portno);
    fflush(stdout);

    struct client_info_structure client_details;
    recv(nsfd,&client_details,sizeof(client_details),0);

    printf("Verifying Client's Ticket with \nIP: %s Portno: %d ticket_no: %d\n"
        ,client_details.IP,client_details.portno,client_details.ticket_no);
    fflush(stdout);
    
    send(ticket_counter_sfd,&client_details,sizeof(client_details),0);

    int status;
    recv(ticket_counter_sfd,&status,sizeof(status),0);

    if(status==1)
        send(nsfd,"Valid Ticket",strlen("Valid Ticket"),0);
    else
        send(nsfd,"Not valid Ticket",strlen("Not valid Ticket"),0);

    printf("Ticket Status %d Sent\n",status);
    fflush(stdout);

    close(nsfd);

    sem_wait(&check_for_client_space);
    no_of_clients--;
    sem_post(&check_for_client_space);

    pthread_exit(NULL);
}

void send_sfd(int nsfd)
{
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
    sendmsg(usfd2, &msg, 0);

    printf("Mall3 transferred sfd to Mall1\n");
    fflush(stdout);
}

void *pass_sfd(void *arg)
{
    while(1)
    {
        char buf[1];
        int nsfd;
        struct iovec iov;
        struct msghdr msg;
        struct cmsghdr *cmsg;
        char cms[CMSG_SPACE(sizeof(int))];

        iov.iov_base = buf;
        iov.iov_len = 1;

        memset(&msg, 0, sizeof msg);
        msg.msg_name = 0;
        msg.msg_namelen = 0;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        msg.msg_control = (caddr_t)cms;
        msg.msg_controllen = sizeof cms;

        if(recvmsg(nusfd, &msg, 0)<0)
            printf("recv error\n");

        cmsg=CMSG_FIRSTHDR(&msg);

        memmove(&nsfd, CMSG_DATA(cmsg), sizeof(int));

        printf("Mall2 send client sfd %d\n",nsfd);
        fflush(stdout);

        sem_wait(&check_for_client_space);
        if(no_of_clients==max_connection_limit)
        {
            send_sfd(nsfd);
        }
        else
        {
            pthread_t thread;
            int *p;
            p=malloc(sizeof(int));
            *p=nsfd;
            pthread_create(&thread,NULL,handling_client,p);
            no_of_clients++;
        }
        sem_post(&check_for_client_space);
    }
}

main()
{
    (void) signal(SIGINT,closesfd);

    sem_init(&check_for_client_space,0,1);

    int portno,clilen,n;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;

    ticket_counter_sfd=socket(AF_INET,SOCK_STREAM,0);
    if(ticket_counter_sfd< 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }

    portno=8888;
    serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(portno);
    if((n=connect(ticket_counter_sfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)))<0) 
    {
        perror("ERROR connecting");
        exit(0);
    }

    printf("Connected to TicketCounter Server\n");
    fflush(stdout);

    //Creating socket for TCP connection
    sfd=socket(AF_INET,SOCK_STREAM,0);
    if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }
    portno=my_portno;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(0);
    }
    
    listen(sfd,max_connection_limit);
    clilen = sizeof(cli_addr);

    printf("Created Socket for client\n");
    fflush(stdout);

    struct sockaddr_un unix_serv_addr,unix_cli_addr;

    usfd=socket(AF_UNIX,SOCK_STREAM,0);
    if (usfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }
    unix_serv_addr.sun_family = AF_UNIX;
    strcpy( unix_serv_addr.sun_path,"/tmp/temp_Mall3");
    unlink(unix_serv_addr.sun_path);
    
    if (bind(usfd,(struct sockaddr *) &unix_serv_addr, sizeof(unix_serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(0);
    }
    
    listen(usfd,1);

    printf("Created Socket for Mall2\n");
    fflush(stdout);

    usfd2=socket(AF_UNIX,SOCK_STREAM,0);
    if(usfd2<0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }

    struct sockaddr_un unix_serv_addr2;
    
    unix_serv_addr2.sun_family = AF_UNIX;
    strcpy(unix_serv_addr2.sun_path,"/tmp/temp_Mall1");
    
    if((n=connect(usfd2,(struct sockaddr *)&unix_serv_addr2, sizeof(unix_serv_addr2)))<0) 
    {
        perror("ERROR connecting");
        exit(0);
    }

    printf("Connected to Mall1\n");
    fflush(stdout);

    int unix_clilen=sizeof(unix_cli_addr);
    nusfd=accept(usfd,(struct sockaddr *)&unix_cli_addr,&unix_clilen);
    
    printf("Mall1 connected\n");
    fflush(stdout);
    
    pthread_t thread;
    pthread_create(&thread,NULL,pass_sfd,NULL);

    while(1)
    {
        int nsfd=accept(sfd,(struct sockaddr *)&cli_addr,&clilen);
        
        sem_wait(&check_for_client_space);
        
        if(max_connection_limit==no_of_clients)
        {
            send_sfd(nsfd);
            close(nsfd);
            continue;
        }

        no_of_clients++;
        
        sem_post(&check_for_client_space);
        
        if(nsfd < 0) 
        {
            perror("ERROR on accept");
            exit(0);
        }

        int* p;
        p=malloc(sizeof(int));
        *p=nsfd;

        pthread_t thread=pthread_create(&thread,NULL,handling_client,p);
    }

    byee();
    
    exit(1);
}