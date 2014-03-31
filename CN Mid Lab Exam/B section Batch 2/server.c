#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

#define numberofconnections 10

void closesfds(int sig);

int sfd;
int portno=12034;

struct sockaddr_in serv_addr, cli_addr[numberofconnections];
int clilen[numberofconnections];

int nsfd[numberofconnections];
int nsfd_size=0;

fd_set fdarray,fdarray2;
int max_fd=0;

void *accept_connections()
{
	FD_ZERO(&fdarray);
	FD_ZERO(&fdarray2);

    int i;
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

        FD_SET(nsfd[i],&fdarray2);

        if(max_fd<nsfd[i])
        	max_fd=nsfd[i];

        printf("Connection created.\n");
    }
}

void *read_from_client()
{
    char buffer[256];
    int i,n,ret;
    struct timeval tv;
    tv.tv_usec=0;
    tv.tv_sec=2;

    while(1)
    {
    	fdarray=fdarray2;
        ret=select(max_fd+1,&fdarray,NULL,NULL,&tv);
        if(ret>0)
        {
        	for(i=0;i<nsfd_size;i++)
        	{
        		if(FD_ISSET(nsfd[i],&fdarray))
        		{
        			n=read(nsfd[i],buffer,100);
        			buffer[n]='\0';
        			if(n<0)
        				printf("error reading \n");
        			else
        				printf("message is : %s \n", buffer);
        		}
        	}
    	}
    }
}

main()
{
	(void)signal(SIGINT,closesfds);

	sfd=socket(AF_INET,SOCK_STREAM,0);
	if (sfd < 0) 
    {
        perror("ERROR in opening socket");
        exit(0);
    }
    
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

void closesfds(int sig)
{
	int i;
	for(i=0;i<nsfd_size;i++)
		close(nsfd[i]);
	close(sfd);
	exit(1);
}