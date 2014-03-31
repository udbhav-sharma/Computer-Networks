#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

main()
{
	int sfd,portno,n;
	char buf[256];
	struct sockaddr_in serv_addr;

	sfd=socket(AF_INET,SOCK_STREAM,0);
	if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }

    portno=8888;
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

    n=recv(sfd,buf,256,0);
    buf[n]='\0';
    printf("Client received: %s\n",buf);
    fflush(stdout);

    n=recv(sfd,buf,256,0);
    buf[n]='\0';
    printf("Client received: %s\n",buf);
    fflush(stdout);

    n=recv(sfd,buf,256,0);
    buf[n]='\0';
    printf("Client received: %s\n",buf);
    fflush(stdout);

    n=recv(sfd,buf,256,0);
    buf[n]='\0';
    printf("Client received: %s\n",buf);
    fflush(stdout);

    close(sfd);
    exit(1);
}