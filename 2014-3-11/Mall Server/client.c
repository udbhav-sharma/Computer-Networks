#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int sfd;

struct addr
{
    char IP[256];
    int portno;
    int ticket_no;
};

struct addr mall;

void closesfd(int sig)
{
    close(sfd);
    printf("Byee\n");
    fflush(stdout);
    exit(1);
}

main()
{
    (void) signal(SIGINT,closesfd);

	int portno,n;
	char buf[256];
	struct sockaddr_in serv_addr;

	sfd=socket(AF_INET,SOCK_STREAM,0);
	if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }

    portno=8889;
    serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(portno);
    if((n=connect(sfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)))<0) 
    {
        perror("ERROR connecting");
        exit(0);
    }

    struct sockaddr_in my_addr;
    int my_len=sizeof(my_addr);

    getsockname(sfd,(struct sockaddr *)&my_addr,&my_len);
    
    char *temp_ip;
    temp_ip=inet_ntoa(my_addr.sin_addr);
    
    char my_IP[255];
    strcpy(my_IP,temp_ip);

    int my_portno=ntohs(my_addr.sin_port);
    printf("MyIp %s\tMyPortno %d\n",my_IP,my_portno);
    fflush(stdout);

    printf("Enter Mall:\n");
    fflush(stdout);

    scanf("%s",buf);

    send(sfd,buf,strlen(buf)+1,0);

    n=recv(sfd,&mall,sizeof(mall),0);
    printf("Mall IP: %s portno: %d ticket_no: %d\n",mall.IP,mall.portno,mall.ticket_no);
    fflush(stdout);
    close(sfd);

    sfd=socket(AF_INET,SOCK_STREAM,0);
    if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }

    portno=mall.portno;
    serv_addr.sin_addr.s_addr=inet_addr(mall.IP);
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(portno);
    if((n=connect(sfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)))<0) 
    {
        perror("ERROR connecting");
        exit(0);
    }

    struct addr my_info;
    my_info.ticket_no=mall.ticket_no;
    my_info.portno=my_portno;
    strcpy(my_info.IP,my_IP);

    send(sfd,&my_info,sizeof(my_info),0);

    char status[255];
    n=recv(sfd,status,255,0);
    status[n]='\0';
    
    printf("Status Received: %s\n",status);
    fflush(stdout);

    close(sfd);
    exit(1);
}