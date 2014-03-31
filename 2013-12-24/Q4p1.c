#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

void handle_signal(int sig)
{
	(void *)signal(SIGALRM,SIG_DFL);
}

main()
{
	int *X,*Y,*pid_1,*pid_2;
	key_t key1=3456,key2=4863,key3=5678,key4=7890;
	
	int shm_X=shmget(key1,4,IPC_CREAT|666);
	if(shm_X==-1)
		printf("Error in creating shared memory for X");
	
	int shm_Y=shmget(key2,4,IPC_CREAT|666);
	if(shm_Y==-1)
		printf("Error in creating shared memory for Y");
	
	int shm_p1=shmget(key3,4,IPC_CREAT|666);
	if(shm_p1==-1)
		printf("Error in creating shared memory for P1");

	int shm_p2=shmget(key4,4,IPC_CREAT|666);
	if(shm_p2==-1)
		printf("Error in creating shared memory for P2");

	Y=(int *)shmat(shm_Y,NULL,0);
	X=(int *)shmat(shm_X,NULL,0);
	pid_1=(int *)shmat(shm_p1,NULL,0);
	pid_2=(int *)shmat(shm_p2,NULL,0);

	*X=0;
	*Y=0;
	*pid_1=getpid();
	while(1)
	{
		sleep(1);
		printf("Process 1 read X %d\n",*X);
		*X=*X+1;
		sleep(1);
		printf("Process 1 written X %d\n",*X);
		kill(*pid_2,SIGALRM);
		printf("Process 1 waiting\n");
		(void *)signal(SIGALRM,handle_signal);
		pause();
		sleep(1);
		printf("Process 1 read Y %d\n",*Y);
	}
	exit(1);
}