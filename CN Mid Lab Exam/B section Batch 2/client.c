#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

char* processes[3]={"./subclient1","./subclient2","./subclient3"};
struct pollfd fdarray[3];
int sfd[3];
int flag[3];

int makeconnection(char* IP,int portno)
{
	int n;
	int temp_sfd=socket(AF_INET,SOCK_STREAM,0);
	if (temp_sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }
	struct sockaddr_in serv_addr;

	serv_addr.sin_addr.s_addr=inet_addr(IP);
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(portno);

    if((n=connect(temp_sfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)))<0) 
    {
        perror("ERROR connecting");
        exit(0);
    }
    return temp_sfd;
}

struct server_details
{
	char IP[255];
	int portno;
};

void closesfd(int sig)
{
    int i;
    for(i=0;i<3;i++)
        close(sfd[i]);
    write(1,"Byee\n",5);
    exit(1);
}

main()
{
    (void )signal(SIGINT,closesfd);
    int i;
    char buf[255];

    FILE* fd[3];
    for(i=0;i<3;i++)
    {
        fd[i]=popen(processes[i],"r");
        if(fd==NULL)
        {
            write(1,"Error: fd is null\n",strlen("Error: fd is null\n"));
            exit(1);
        }
        fdarray[i].fd=fileno(fd[i]);
        fdarray[i].events=POLLRDNORM;
        fdarray[i].revents=0;
        flag[i]=1;
    }

    while(1)
    {
    int ret=poll(fdarray,3,1);
    for(i=0;i<3;i++)
    {
        if(fdarray[i].revents & (POLLRDNORM | POLLERR))
        {
            if(flag[i])
            {
                struct server_details server;
                fscanf(fd[i],"%s %d",server.IP,&server.portno);
                printf("Client read:\t%s %d\n",server.IP,server.portno);
                sfd[i]=makeconnection(server.IP,server.portno);
                flag[i]=0;
            }
            else
            {
                fscanf(fd[i],"%s",buf);
                printf("Client read:\t%s\n",buf);
                send(sfd[i],buf,strlen(buf)+1,0);
            }
        }
    }
    }
    for(i=0;i<3;i++)
    close(sfd);
    exit(1);
}