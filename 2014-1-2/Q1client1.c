#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "SystemVSemaphores.c"
#define clients 4
#define msglength 60

main()
{
	char msg[msglength];
	int sem_read=sem_init(clients,0);
	int sem_write=sem_init(clients,0);
	int clientpipe=open("file_client1",O_RDWR);
	pid_t pid=fork();
	if(pid>0)
	{
		while(1)
		{
			scanf("%s",msg);
			printf("Read msg: %s\n",msg);
			write(clientpipe,msg,strlen(msg)+1);
			sem_change(sem_read,1,1);
		}
	}
	else if(pid==0)
	{
		char msg[msglength];
		while(1)
		{
			sem_change(sem_write,1,-1);
			read(clientpipe,msg,msglength);
			printf("Client read: %s\n",msg);
		}
	}
	else
	{
		printf("Error in creating Child process.\n");
		exit(0);
	}
	exit(1);
}