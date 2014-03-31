#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include "SystemVSemaphores.c"

main()
{
	int sem_X=sem_init(1,0);
	int sem_Y=sem_init(1,0);
	while(1)
	{
		printf("Process 1 Reading X\n");
		printf("Process 1 Writing X\n");
		sleep(1);
		sem_change(sem_Y,0,1);
		printf("Process 1 Waiting\n");
		sem_change(sem_X,0,-1);
		printf("Process 1 Reading Y\n");
		sleep(1);
	}
	exit(1);
}