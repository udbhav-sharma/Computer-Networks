#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "SystemVSemaphores.c"
#define clients 4
#define msglength 60

main()
{
	int sem_read=sem_init(clients,0);
	int sem_write=sem_init(clients,0);

	key_t key1=9056;
	int shm_client=shmget(key1,sizeof(msglength),IPC_CREAT|666);
	if(shm_client==-1)
		printf("Error in creating shared memory client in client 0.\n");
	char *msg=shmat(shm_client,NULL,0);
	
	pid_t pid=fork();
	if(pid>0)
	{
		while(1)
		{
			scanf("%s",msg);
			printf("Read msg: %s\n",msg);
			sem_change(sem_read,3,1);
		}
	}
	else if(pid==0)
	{
		while(1)
		{
			sem_change(sem_write,3,-1);
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