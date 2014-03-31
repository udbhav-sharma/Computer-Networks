#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <pthread.h>
#include <poll.h>
#include <semaphore.h>

#define numberofconnections 10

int sfd,portno;
struct sockaddr_in serv_addr, cli_addr[numberofconnections];
int clilen[numberofconnections];
int nsfd[numberofconnections];
int nsfd_size=0;
sem_t mutex;
struct pollfd fdarray[numberofconnections];

void *accept_connections()
{
    int i;
    int flag=1;
    for(i=0;i<numberofconnections;i++)
    {
        clilen[i]=sizeof(cli_addr[i]);
        nsfd[i]=accept(sfd,(struct sockaddr *)&cli_addr[i],&clilen[i]);
        nsfd_size++;
        if(nsfd[i]<0) 
        {
            perror("ERROR on accept");
            exit(0);
        }
        printf("Connection created.\n");
        
        fdarray[i].fd=nsfd[i];
        fdarray[i].events=POLLRDNORM;
        fdarray[i].revents=0;

        sem_post(&mutex);
    }
}

void *read_from_client()
{
    printf("Thread waiting for mutex\n");
    sem_wait(&mutex);
    printf("Thread acquired mutex\n");
    char buffer[256];
    int n;
    while(1)
    {
        printf("Entered loop\n");
        int ret=poll(fdarray,nsfd_size,1);
        int i;
        for(i=0;i<nsfd_size;i++)
        {
            //sleep(10);
            if(fdarray[i].revents & (POLLRDNORM | POLLERR))
            {
                n=read(fdarray[i].fd,buffer,100);
                if(n<0)
                    printf("error reading \n");
                else
                    printf("message is : %s \n", buffer);
            }
        }
    }
}

main()
{
    sem_init(&mutex,0,0);
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }
    portno =8888;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(0);
    }
    
    listen(sfd,numberofconnections);

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
    int i;
    for(i=0;i<nsfd_size;i++)
        close(nsfd[i]);
    close(sfd);
	exit(1);
}