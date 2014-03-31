#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>

int sfd;
int portno=9089;

void closesfd(int sig)
{
    close(sfd);
}

struct token
{
    char receiver_IP[255];
    int receiver_portno;
    char sender_IP[255];
    int sender_portno;
    char msg[255];
    int writable;
};

main()
{
    (void) signal(SIGINT,closesfd);

	int n;
	struct sockaddr_in serv_addr;

	sfd=socket(AF_INET,SOCK_STREAM,0);
	if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }

    serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(portno);

    if((n=connect(sfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)))<0) 
    {
        perror("ERROR connecting");
        exit(0);
    }
    else
    {
        printf("Connection Done with Server.\n");
        fflush(stdout);
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

    fd_set r_fds,r_fds2;
    FD_ZERO(&r_fds);
    FD_ZERO(&r_fds2);

    FD_SET(0,&r_fds2);
    FD_SET(sfd,&r_fds2);

    struct timeval tv;
    tv.tv_sec=2;
    tv.tv_usec=0;

    int writing_flag=0;
    char receiver_IP[255];
    int receiver_portno;
    char msg[255];

    while(1)
    {
        r_fds=r_fds2;
        int ret=select(sfd+1,&r_fds,NULL,NULL,&tv);
        if(ret>0)
        {
            if(FD_ISSET(0,&r_fds))
            {
                scanf("%s",receiver_IP);
                scanf("%d",&receiver_portno);
                scanf("%s",msg);
                writing_flag=1;
            }
            else
            {
                struct token temp_token;
                recv(sfd,&temp_token,sizeof(temp_token),0);
                // printf("Token contents are:\nReceiver IP: %s\tReceiver Portno: %d\nSender IP: %s\tSender Portno: %d\nMsg: %s\n",
                //        temp_token.receiver_IP,temp_token.receiver_portno,temp_token.sender_IP,temp_token.sender_portno,temp_token.msg);
                // fflush(stdout);
                if(temp_token.writable)
                {
                    if(writing_flag)
                    {
                        temp_token.writable=0;
                        
                        strcpy(temp_token.receiver_IP,receiver_IP);
                        temp_token.receiver_portno=receiver_portno;
                        
                        strcpy(temp_token.msg,msg);
                        
                        strcpy(temp_token.sender_IP,my_IP);
                        temp_token.sender_portno=my_portno;

                        printf("Your msg sent\n");
                        fflush(stdout);
                        
                        writing_flag=0;
                    }
                    else
                    {
                        //Do nothing
                    }                   
                }
                else
                {
                    if(strcmp(temp_token.receiver_IP,my_IP)==0 && temp_token.receiver_portno==my_portno)
                    {
                        printf("Msg for you from %s %d\n%s\n",temp_token.receiver_IP,temp_token.receiver_portno,temp_token.msg);
                        fflush(stdout);
                    }
                    else if(strcmp(temp_token.sender_IP,my_IP)==0 && temp_token.sender_portno==my_portno)
                    {
                        strcpy(temp_token.receiver_IP,"");
                        temp_token.receiver_portno=0;
                        
                        strcpy(temp_token.msg,msg);
                        
                        strcpy(temp_token.sender_IP,"");
                        temp_token.sender_portno=0;
                        temp_token.writable=1;

                    }
                }
                send(sfd,&temp_token,sizeof(temp_token),0);
            }
        }
    }
    close(sfd);
    exit(1);
}