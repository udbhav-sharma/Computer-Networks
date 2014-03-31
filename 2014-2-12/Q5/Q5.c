#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#define maxnumberofprocesses 2
#define maxnumberofthreads 2

pthread_t thread_ids[maxnumberofthreads];
int thread_running[maxnumberofthreads];
int thread_running_size=0;
int thread_sfds[maxnumberofthreads];

void *handleclient(void *arg)
{
    int index=(*(int *)(arg));
    char buf[100];
    while(1)
    {
    	//write(1,"Thread running\n",15);
   		//printf("thread running index %d\n",thread_running[index]);
        if(thread_running[index]==1)
        {
            write(1,"Thread reading\n",15);
            read(thread_sfds[index],buf,100);
            write(1,buf,strlen(buf)+1);
        }
        else
        {
            sleep(2);
        }
    }
}

void createthread()
{
    int i;
    for(i=0;i<maxnumberofthreads;i++)
    {
        thread_running[i]=0;
        int *p=malloc(sizeof(int));
        *p=i;
        pthread_create(&thread_ids[i],NULL,(void *)handleclient,p);
        //sleep(3);
    }
    write(1,"Threads created\n",16);
}

void acceptconnection(int sfd)
{
    int i;
    while(1)
    {
        if(thread_running_size<maxnumberofthreads)
        {
            struct sockaddr_in cli_addr;
            int clilen=sizeof(cli_addr);
            int nsfd=accept(sfd,(struct sockaddr *)&cli_addr,&clilen);
            //Assign thread nsfd;
            write(1,"Connection created\n",19);
            for(i=0;i<maxnumberofthreads;i++)
                if(thread_running[i]==0)
                {
                    thread_sfds[i]=nsfd;
                    thread_running[i]=1;
                    thread_running_size++;
                    write(1,"Thread assigned\n",16);
                    break;
                }
            //Wake up thread
        }
    }
}

main()
{
	int sfd,nsfd,portno,clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;

	sfd=socket(AF_INET,SOCK_STREAM,0);
	if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }
    portno =9999;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(0);
    }
    
    listen(sfd,maxnumberofthreads*maxnumberofprocesses);

    int i;
    for(i=0;i<maxnumberofprocesses;i++)
    {
    	write(1,"Fork called\n",12);
        int pid=fork();
        if(pid==0)
        {
            createthread();
            acceptconnection(sfd);
            break;
        }
    }
    wait(NULL);
	exit(1);
}