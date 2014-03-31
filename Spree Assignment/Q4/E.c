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
#include <errno.h>

#define maxnumberofconnections 5
#define C_PORT 21000
#define S_PORT 22000
#define E_PORT 23000
#define TIS_PORT 20000

int sfd_tcp;
int sfd_udp;
int sfd_tis;
int vacant_slots=maxnumberofconnections-1;

struct sockaddr_in server_TIS,server_S,server_C;

void closesfd(int sig)
{
    close(sfd_tis);
    close(sfd_tcp);
    close(sfd_udp);
    write(1,"Bye\n",strlen("Bye\n"));
    exit(1);
}

void initialize_server_addr()
{   
    server_TIS.sin_family = AF_INET;
    server_TIS.sin_addr.s_addr =inet_addr("127.0.0.1");
    server_TIS.sin_port=htons(TIS_PORT);

    server_S.sin_family = AF_INET;
    server_S.sin_addr.s_addr =inet_addr("127.0.0.1");
    server_S.sin_port=htons(S_PORT);

    server_C.sin_family = AF_INET;
    server_C.sin_addr.s_addr =inet_addr("127.0.0.1");
    server_C.sin_port=htons(C_PORT);
}

struct msg{
    char client_ip[255];
    char client_ticket[255];
};

int check_for_validity(char* client_ip,char ticket[255])
{   
    struct msg t;
    strcpy(t.client_ip,client_ip);
    strcpy(t.client_ticket,ticket);

    send(sfd_tis,&t,sizeof(t),0);
   
    write(1,"Send IP and Ticket\n",strlen("Send Ip and Ticket\n"));
    
    char buf[255];
    recv(sfd_tis,buf,255,0);
    
    write(1,"Return value:\t",strlen("Return value:\t"));
    write(1,buf,strlen(buf));
    write(1,"\n",1);

    if(strcmp(buf,"1")==0)
        return 1;
    else
        return 0;
} 

int check_for_other_malls_slots()
{
    char buf[255];
    int cli_len=sizeof(server_S);
    sendto(sfd_udp,"query\0",strlen("query")+1,0,
        (struct sockaddr *) &server_S,cli_len);

    recvfrom(sfd_udp,buf,255,0,
        (struct sockaddr *) &server_S,&cli_len);
    if(strcmp(buf,"1")==0)
        return 1;

    cli_len=sizeof(server_C);
    sendto(sfd_udp,"query\0",strlen("query")+1,0,
        (struct sockaddr *) &server_C,cli_len);

    recvfrom(sfd_udp,buf,255,0,
        (struct sockaddr *) &server_C,&cli_len);
    if(strcmp(buf,"1")==0)
        return 2;
    return 0;
}

void* handle_mall_connection(void *arg)
{
    struct sockaddr_in var_serv_addr=(*(struct sockaddr_in *)arg);
    int cli_len=sizeof(var_serv_addr);

    char query[255];
    while(1)
    {
        recvfrom(sfd_udp,query,255,0,
            (struct sockaddr *) &var_serv_addr,&cli_len);
        if(vacant_slots!=0)
            sendto(sfd_udp,"1\0",strlen("1")+1,0,
                (struct sockaddr *) &var_serv_addr,cli_len);
        else
            sendto(sfd_udp,"0\0",strlen("0")+1,0,
                (struct sockaddr *) &var_serv_addr,cli_len);
    }
}

void socket_for_client()
{
    int portno;
    struct sockaddr_in serv_addr;

    sfd_tcp=socket(AF_INET,SOCK_STREAM,0);
    if (sfd_tcp<0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }

    portno=11000;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port=htons(portno);

    if (bind(sfd_tcp,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding in tcp socket");
        exit(0);
    }

    listen(sfd_tcp,maxnumberofconnections);
}

void socket_for_udp()
{
    int portno;
    struct sockaddr_in serv_addr;

    sfd_udp=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (sfd_udp<0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }
    portno=E_PORT;
    serv_addr.sin_family =AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port=htons(portno);

    if (bind(sfd_udp,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding in udp socket");
        exit(0);
    }
}

void socket_for_tis()
{
    int n;
    sfd_tis=socket(AF_INET,SOCK_STREAM,0);

    if (sfd_tis<0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }
    if((n=connect(sfd_tis,(struct sockaddr *)&server_TIS, sizeof(server_TIS)))<0) 
    {
        perror("ERROR connecting");
        exit(0);
    }
}

main()
{
    (void)signal(SIGINT,closesfd);
    initialize_server_addr();
    socket_for_client();
    socket_for_udp();
    socket_for_tis();
    
    //Pthreads handling communication between mall servers
    pthread_t S_pid,C_pid;
    pthread_create(&S_pid,NULL,handle_mall_connection,(struct sockaddr_in *)&(server_S));
    pthread_create(&C_pid,NULL,handle_mall_connection,(struct sockaddr_in *)&(server_C));

    while(1)
    {
        struct sockaddr_in cli_addr;
        int clilen = sizeof(cli_addr);
        int nsfd=accept(sfd_tcp,(struct sockaddr *)&cli_addr,&clilen);
        vacant_slots--;
        if(nsfd<0) 
        {
            perror("ERROR on accept");
            exit(0);
        }
        char ticket[255];
        recv(nsfd,ticket,255,0);
        char *temp_ip;
        temp_ip=inet_ntoa(cli_addr.sin_addr);
        if(check_for_validity(temp_ip,ticket))
        {
            if(vacant_slots>0)
                send(nsfd,"Valid Ticket\0",strlen("Valid Ticket")+1,0);
            else 
            {
                int ret=check_for_other_malls_slots();
                if(ret==1)
                {
                    send(nsfd,"127.0.0.1\0",strlen("127.0.0.1")+1,0);
                    sleep(2);
                    send(nsfd,"10000\0",strlen("10000")+1,0);
                }
                else if(ret==2)
                {
                    send(nsfd,"127.0.0.1\0",strlen("127.0.0.1")+1,0);
                    sleep(2);
                    send(nsfd,"9000\0",strlen("9000")+1,0);
                }
                else
                {
                    send(nsfd,"No slots\0",strlen("No slots")+1,0);
                }
            }

        }
        else
            send(nsfd,"Invalid Ticket\0",strlen("Invalid Ticket")+1,0);
    }

    close(sfd_tis);
    close(sfd_tcp);
    close(sfd_udp);
    exit(1);
}