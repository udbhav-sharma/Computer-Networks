#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
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

int openpipe(int i)
{
	char pipename[40]="file_client";
	char pipename2[10];
	int j=i;
	int k=0;
	while(j>0)
	{
		j=j/10;
		k++;
	}
	j=i;
	pipename2[k]='\0';
	while(k>0)
	{
		pipename2[--k]=j%10+'0';
		j=j/10;
	}
	strcat(pipename,pipename2);
	int client=open(pipename,O_RDWR);
	return client;
}

void closepipe(int client)
{
	close(client);
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
		printf("Error in creating shared memory for X\n");

	X=(struct data *)shmat(shm_X,NULL,0);

	for(i=0;i<clients;i++)
	{
		p=fork();
		if(p>0)
			pid[i]=p;
		else if(p==0)
			break;
		else
		{
			printf("Error in creating child process.\n");
			exit(0);
		}
	}
	if(p==0)
	{
		int num=i;
		int client=openpipe(num);
		char msg[msglength];
		while(1)
		{
			printf("Child Process %d waiting for reading\n",num);
			sem_change(sem_read,num,-1);
			read(client,msg,msglength);
			printf("Child Process %d read msg %s\n",num,msg);
			X->n=num;
			strcpy(X->msg,msg);
			sem_change(parent_write,0,1);
		}
	}
	else
	{
		while(1)
		{
			int j;
			printf("Parent process Waiting\n");
			sem_change(parent_write,0,-1);
			printf("Parent process read %d %s\n",X->n,X->msg);
			for(j=0;j<clients;j++)
			{
				if(X->n==j)
					continue;
				int client=openpipe(j);
				write(client,X->msg,strlen(X->msg)+1);
				closepipe(client);
				sem_change(sem_write,j,1);
			}
		}
	}
	exit(1);
}