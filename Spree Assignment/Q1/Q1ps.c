#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define maxnumberofconnections 10
#define maxnumberofresources 100

int sfd,portno;
struct sockaddr_in serv_addr, cli_addr[maxnumberofconnections];
int clilen[maxnumberofconnections];
int nsfd[maxnumberofconnections];
int nsfd_size=0;
int max_fd=0;

fd_set fdarray,fdarray2;

void *accept_connections()
{
    FD_ZERO(&fdarray);
    FD_ZERO(&fdarray2);

    for(nsfd_size=0;nsfd_size<maxnumberofconnections;nsfd_size++)
    {
        clilen[nsfd_size]=sizeof(cli_addr[nsfd_size]);
        nsfd[nsfd_size]=accept(sfd,(struct sockaddr *)&cli_addr[nsfd_size],&clilen[nsfd_size]);
        
        if(nsfd[nsfd_size]<0) 
        {
            perror("ERROR on accept");
            exit(0);
        }
        
        FD_SET(nsfd[nsfd_size],&fdarray);
        FD_SET(nsfd[nsfd_size],&fdarray2);

        if(max_fd<nsfd[nsfd_size])
            max_fd=nsfd[nsfd_size];

        write(1,"Connection Created\n",strlen("Connection Created\n"));
    }
}

struct Resources_details{
    char name[255];
    int instances;
};

struct process_details{
    char name[255];
};

struct Resources_details available[maxnumberofresources];
int available_size=0;
int** request;
int** allocation;
struct process_details process[maxnumberofconnections];
int Deadlock_process;

int get_resource_index(char resource_name[255])
{
    int i;
    for(i=0;i<available_size;i++)
        if(strcmp(available[i].name,resource_name)==0)
            return i;
    return -1;
}

int get_process_index(char process_name[255])
{
    int i;
    for(i=0;i<maxnumberofconnections;i++)
        if(strcmp(process[i].name,process_name)==0)
            return i;
    return -1;
}

int check_for_deadlock()
{
    int i,j;
    int Finish[maxnumberofconnections];

    //Step 1
    for(i=0;i<maxnumberofconnections;i++)
    {
        for(j=0;j<available_size;j++)
            if(allocation[i][j]!=0)
                break;
        if(j!=available_size)
            Finish[i]=0;
        else
            Finish[i]=1;
    }

    //Step 2
    int flag=-1;
    for(i=0;i<maxnumberofconnections;i++)
        if(Finish[i]==0)
        {
            for(j=0;j<available_size;j++)
                if(available[j].instances<request[i][j])
                    break;
            if(j==available_size)
                flag=j;
        }

    //Step 4
    if(flag==-1)
    {
        for(i=0;i<maxnumberofconnections;i++)
            if(!Finish[i])
                return 0;
    }
    return 1;

}

void Preempt_resources()
{
    int j;
    for(j=0;j<available_size;j++)
    {
       if(allocation[Deadlock_process][j]==1)
            available[j].instances+=allocation[Deadlock_process][j];
        allocation[Deadlock_process][j]=0;
        request[Deadlock_process][j]=0;
    }
}

int handle_resource_request(char process_name[255],char resource_name[255])
{
    int i=get_process_index(process_name);
    int j=get_resource_index(resource_name);

    request[i][j]=1;
    if(!check_for_deadlock())
    {
        Deadlock_process=i;
        Preempt_resources();
        return 0;
    }
    else
    {
        if(available[j].instances==0)
            return 2;

        available[j].instances-=1;
        allocation[i][j]+=1;
        request[i][j]=0;
        return 1;
    }
}

void remove_allocation(char process_name[255],char resource_name[255])
{
    int i=get_process_index(process_name);
    int j=get_resource_index(resource_name);

    if(allocation[i][j]!=0)
    {
        available[j].instances+=allocation[i][j];
        allocation[i][j]=0;
    }
    else
        request[i][j]=0;
}

void initialize_available()
{
    char buffer[255];
    FILE* fd=fopen("Resources","r");
    while(available_size<5)
    {
        fscanf(fd,"%s",buffer);
        strcpy(available[available_size].name,buffer);
        available[available_size].instances=1;
        available_size++;
    }
}

void initialize_request()
{
    int i,j;
    request=(int**)malloc(sizeof(int*)*maxnumberofconnections);
    for(i=0;i<maxnumberofconnections;i++)
        request[i]=(int*)malloc(sizeof(int)*maxnumberofresources);

    for(i=0;i<maxnumberofconnections;i++)
        for(j=0;j<maxnumberofresources;j++)
            request[i][j]=0;
}

void initialize_allocation()
{
    int i,j;

    allocation=(int**)malloc(sizeof(int*)*maxnumberofconnections);
    for(i=0;i<maxnumberofconnections;i++)
        allocation[i]=(int*)malloc(sizeof(int)*maxnumberofresources);

    for(i=0;i<maxnumberofconnections;i++)
        for(j=0;j<maxnumberofresources;j++)
            allocation[i][j]=0;
}

void initialize_process()
{
    int i;
    char *v[]={"1","2","3","4","5","6","7","8","9","10"};
    for(i=0;i<maxnumberofconnections;i++)
    {
        strcpy(process[i].name,"p");
        strcat(process[i].name,v[i]);
    }
}

void *read_from_client()
{
    char buffer[256];

    initialize_available();
    initialize_request();
    initialize_allocation();
    initialize_process();

    struct timeval tv;
    tv.tv_usec=0;
    tv.tv_sec=2;

    while(1)
    {
        fdarray=fdarray2;

        int ret=select(max_fd+1, &fdarray, NULL, NULL,&tv);
        int i;
        if(ret>0)
        {
            for(i=0;i<nsfd_size;i++)
            {
                if(FD_ISSET(nsfd[i],&fdarray))
                {
                    char name[255];
                    char resource[255];

                    recv(nsfd[i],buffer,256,0);
                    recv(nsfd[i],name,255,0);
                    recv(nsfd[i],resource,255,0);


                    write(1,buffer,strlen(buffer));
                    write(1,"\n",1);
                    write(1,name,strlen(name));
                    write(1,"\n",1);
                    write(1,resource,strlen(resource));
                    write(1,"\n",1);

                    //Removal request handling
                    if(strcmp(buffer,"Remove")==0)
                    {
                        remove_allocation(name,resource);
                        send(nsfd[i],"Removed\0",strlen("Removed")+1,0);
                        write(1,"Sent status\n",strlen("Sent status\n"));
                        continue;
                    }

                    //allocation request handling
                    int k=handle_resource_request(name,resource);
                    write(1,"No segmentation fault\n",strlen("No segmentation fault\n")+1);
                    
                    if(k==1)
                        send(nsfd[i],"Allocated\0",strlen("Allocated")+1,0);
                    else if(k==2)
                        send(nsfd[i],"Wait\0",strlen("Wait")+1,0);
                    else
                    {
                        write(1,"Deadlock\n",strlen("Deadlock\n"));
                        int j;
                        for(j=0;j<nsfd_size;j++)
                            send(nsfd[j],"Deadlock\0",strlen("Deadlock"),+10);

                        sleep(2);
                        for(j=0;j<nsfd_size;j++)
                            if(j!=Deadlock_process)
                                send(nsfd[j],"Continue\0",strlen("Continue")+1,0);
                            else
                                send(nsfd[j],"Preempt\0",strlen("Preempt")+1,0);
                    }
                    write(1,"Sent status\n",strlen("Sent status\n"));
                }
            }
        }
    }
}

void close_all_sfds()
{
    int i;
    for(i=0;i<nsfd_size;i++)
        close(nsfd[i]);
    close(sfd);
    write(1,"Hv a Wonderful Day\n",strlen("Hv a Wonderfull Day\n"));
}

void closesfd(int sig)
{
    close_all_sfds();
    exit(1);
}

main()
{
    (void )signal(SIGINT,closesfd);

	sfd=socket(AF_INET,SOCK_STREAM,0);
	if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }
    portno =9006;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(0);
    }
    
    listen(sfd,maxnumberofconnections);

    //Making threads to handle accept and read
    pthread_t connection_thread,read_thread;
    int error=pthread_create(&connection_thread,NULL,accept_connections,NULL);
    if(error<0)
    {
        perror("ERROR in connection_thread:");
        exit(0);
    }
    error=pthread_create(&read_thread,NULL,read_from_client,NULL);
    if(error<0)
    {
        perror("ERROR in read_thread:");
        exit(0);
    }
    pthread_join( connection_thread, NULL);
    pthread_join( read_thread, NULL);

    //Closing the open sockets
    close_all_sfds();

	exit(1);
}