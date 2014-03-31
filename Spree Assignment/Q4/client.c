#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>

int sfd;

void closesfd(int sig)
{
    close(sfd);
    write(1,"Bye\n",strlen("Bye\n"));
    exit(1);
}

void create_new_socket(char server_ip[255],int portno)
{
    int n;
    struct sockaddr_in serv_addr;

    sfd=socket(AF_INET,SOCK_STREAM,0);
    if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }

    serv_addr.sin_addr.s_addr=inet_addr(server_ip);
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(portno);
    if((n=connect(sfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)))<0) 
    {
        perror("ERROR connecting");
        exit(0);
    }
}

main()
{
    (void)signal(SIGINT,closesfd);
	int portno;
	char buf[256];
	struct hostent *server;

    create_new_socket("127.0.0.1",8888);

    char mall[20];
    write(1,"Mall Name\n",strlen("Mall Name\n"));
    scanf("%s",mall);
    send(sfd,mall,strlen(mall)+1,0);

    char server_ip[255];
    char server_port[255];
    char ticket[255];

    recv(sfd,server_ip,255,0);
    recv(sfd,server_port,255,0);
    recv(sfd,ticket,255,0);

    int i;
    portno=0;
    for(i=0;i<strlen(server_port);i++)
        portno=portno*10+(server_port[i]-'0');

    printf("Data Received:\t%s %d\n",server_ip,portno);

    close(sfd);
    create_new_socket(server_ip,portno);

    write(1,"Connection created\n",strlen("Connection created\n"));
    send(sfd,ticket,strlen(ticket)+1,0);
    write(1,"Ticket Send\n",strlen("Ticket Send\n"));
    char ticket_status[255];
    recv(sfd,ticket_status,255,0);
    write(1,"Ticket Status:\t",strlen("Ticket Status:\t"));
    write(1,ticket_status,strlen(ticket_status));
    write(1,"\n",1);
    
    if(strcmp(ticket_status,"No slots")==0 || strcmp(ticket_status,"Valid Ticket")==0 
        || strcmp(ticket_status,"Invalid Ticket")==0)
        exit(1);
    else
    {
        recv(sfd,server_port,255,0);
        portno=0;
        for(i=0;i<strlen(server_port);i++)
            portno=portno*10+(server_port[i]-'0');

        write(1,"portno:\t",strlen("portno:\t"));
        write(1,server_port,strlen(server_port));
        write(1,"\n",1);
        
        close(sfd);
        create_new_socket(ticket_status,portno);
        
        write(1,"Connection created\n",strlen("Connection created\n"));
        send(sfd,ticket,strlen(ticket)+1,0);
        write(1,"Ticket Send\n",strlen("Ticket Send\n"));
        char ticket_status[255];
        recv(sfd,ticket_status,255,0);
        write(1,"Ticket Status:\t",strlen("Ticket Status:\t"));
        write(1,ticket_status,strlen(ticket_status));
        write(1,"\n",1);
    }

    exit(1);
}