#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "SystemVSemaphores.c"
#define clients 4
#define msglength 60

struct data{
	int n;
	char msg[msglength];
};

char* openshm(int num,key_t* keys)
{
	int shm_client=shmget(keys[num],sizeof(msglength),IPC_CREAT|666);
	if(shm_client==-1)
		printf("Error in creating shared memory client in process %d.\n",num);
	char *msg=shmat(shm_client,NULL,0);
	return msg;
}

main()
{
	int i;
	struct data *X;

	int sem_read=sem_init(clients,0);
	int sem_write=sem_init(clients,0);
	int parent_write=sem_init(1,0);

	pid_t p,*pid;
	pid=(pid_t*)malloc(clients*sizeof(pid_t));

	key_t key1=4023;
	int shm_X=shmget(key1,sizeof(struct data),IPC_CREAT|666);
	if(shm_X==-1)
		printf("Error in creating shared memory for X.\n");

	X=(struct data *)shmat(shm_X,NULL,0);

	key_t *keys=(key_t*)malloc(clients*sizeof(key_t));
	keys[0]=4576;
	keys[1]=2345;
	keys[2]=3454;
	keys[3]=9056;

	for(i=0;i<clients;i++)
	{
		p=fork();
		if(p>0)
			pid[i]=p;
		else if(p==0)
			break;
		else
		{
			printf("Error in creating child process %d.\n",i);
			exit(0);
		}
	}
	if(p==0)
	{
		int num=i;
		char *readmsg=openshm(num,keys);
		while(1)
		{
			printf("Child Process %d waiting for reading.\n",num);
			sem_change(sem_read,num,-1);
			printf("Child Process %d read msg %s.\n",num,readmsg);
			X->n=num;
			strcpy(X->msg,readmsg);
			sem_change(parent_write,0,1);
		}
	}
	else
	{
		int j;
		char *writtenmsg[clients];
		for(j=0;j<clients;j++)
			writtenmsg[j]=openshm(j,keys);

		while(1)
		{
			printf("Parent process Waiting.\n");
			sem_change(parent_write,0,-1);
			printf("Parent process read %d %s.\n",X->n,X->msg);
			for(j=0;j<clients;j++)
			{
				if(X->n==j)
					continue;
				strcpy(writtenmsg[j],X->msg);
				sem_change(sem_write,j,1);
			}
		}
	}
	exit(1);
}