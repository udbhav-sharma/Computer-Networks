#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include "SystemVSemaphores.c"

main()
{
	int *X,*Y;
	int sem_X=sem_init(1,0);
	int sem_Y=sem_init(1,0);
	key_t key1=3456,key2=4863;
	
	int shm_X=shmget(key1,4,IPC_CREAT|666);
	if(shm_X==-1)
		printf("Error in creating shared memory for X");
	
	int shm_Y=shmget(key2,4,IPC_CREAT|666);
	if(shm_Y==-1)
		printf("Error in creating shared memory for Y");
	
	Y=(int *)shmat(shm_Y,NULL,0);
	X=(int *)shmat(shm_X,NULL,0);

	*X=0;
	*Y=0;
	while(1)
	{
		sleep(1);
		printf("Process 1 read X %d\n",*X);
		*X=*X+1;
		sleep(1);
		printf("Process 1 written X %d\n",*X);
		sem_change(sem_Y,0,1);
		printf("Process 1 waiting\n");
		sem_change(sem_X,0,-1);
		sleep(1);
		printf("Process 1 read Y %d\n",*Y);
	}
	exit(1);
}