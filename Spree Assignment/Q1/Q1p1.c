#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h> 
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define maxnumberofresources 100

int sfd;

void closesfd(int sig)
{
    close(sfd);
    write(1,"Hv a Wonderful Day\n",strlen("Hv a Wonderfull Day\n"));
    exit(1);
}

struct resources_structure
{
    char name[100];
    int flag;
};

struct resources_structure RES[maxnumberofresources];
int RES_size=0;

void show_resources_allocated()
{
    int i;
    printf("------------------Following Resources are Allocated--------------\n");
    for(i=0;i<RES_size;i++)
        printf("%s %d\n",RES[i].name,RES[i].flag);
}

void modify_resource(char resource[255],int flag)
{
    int i;
    for(i=0;i<RES_size;i++)
        if(strcmp(resource,RES[i].name)==0)
            RES[i].flag=flag;
}

void removeall()
{
    int i;
    for(i=0;i<RES_size;i++)
        RES[i].flag=0;
}

main()
{
    (void)signal(SIGINT,closesfd);

	int portno,n;
	char buf[256];
	struct sockaddr_in serv_addr;
	struct hostent *server;

    FILE* fd=fopen("Resources","r");
    while(RES_size<5)
    {
        fscanf(fd,"%s",buf);
        strcpy(RES[RES_size].name,buf);
        RES_size++;
    }

	sfd=socket(AF_INET,SOCK_STREAM,0);
	if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }

    portno=9006;
    serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(portno);
    if((n=connect(sfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)))<0) 
    {
        perror("ERROR connecting");
        exit(0);
    }
    
    fd_set fdarray;
    FD_ZERO(&fdarray);

    write(1,"For adding a resource enter 1 and for removing a resource enter -1\n",
        strlen("For adding a resource enter 1 and for removing a resource enter -1\n"));
    
    char actiontype[255];
    char resource[255];
    char status[255];
    char status_sent[255];
    
    removeall();
    show_resources_allocated();
    while(1)
    {
        FD_SET(0,&fdarray);
        FD_SET(sfd,&fdarray);

        struct timeval tv;
        tv.tv_usec=0;
        tv.tv_sec=2;

        int ret=select(sfd+1, &fdarray, NULL, NULL,&tv);

        if(ret>0 && FD_ISSET(0,&fdarray))
        {
            scanf("%s",actiontype);
            scanf("%s",resource);

            strcpy(status_sent,"Requesting");
            if(strcmp("1",actiontype)==0)
                send(sfd,status_sent,strlen(status_sent)+1,0);

            else if(strcmp("-1",actiontype)==0)
            {
                strcpy(status_sent,"Remove");
                send(sfd,status_sent,strlen(status_sent)+1,0);
            }

            strcpy(status_sent,"p1");
            sleep(2);
            send(sfd,status_sent,strlen(status_sent)+1,0);
            sleep(2);
            send(sfd,resource,strlen(resource)+1,0);
            write(1,"Sent Data\n",strlen("Sent Data\n"));
        }

        else if(ret>0)
        {
            int k=recv(sfd,status,255,0);
            status[k]='\0';

            write(1,status,strlen(status));
            write(1,"\n",1);
            if(strcmp("Allocated",status)==0)
            {
                modify_resource(resource,1);
                strcpy(resource,"");
            }
            else if(strcmp("Wait",status)==0)
            {
                modify_resource(resource,2);
                strcpy(resource,"");
            }
            else if(strcmp("Deadlock",status)==0)
            {
                if(strcmp(resource,"")!=0)
                    modify_resource(resource,1);

                char status2[255];
                recv(sfd,status2,255,0);
                write(1,status2,strlen(status2));
                write(1,"\n",1);
                if(strcmp("Continue",status2)==0)
                {}
                else
                {
                    removeall();
                }

            }
            else if(strcmp("Removed",status)==0)
                 modify_resource(resource,0);
            
            show_resources_allocated();
        }
    }
    close(sfd);
    exit(1);
}
