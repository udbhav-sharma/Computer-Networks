#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

int sharedX=0,sharedY=0;
sem_t mutex_thread_1,mutex_thread_2;

void* thread_1_func()
{
	while(1)
	{
		//Critical Section
		printf("Thread 1 read Y as %d\n",sharedY);
		sleep(1);
		printf("Thread 1 wrote X as %d\n",++sharedX);
		sem_post(&mutex_thread_2);
		sem_wait(&mutex_thread_1);
	}
}

void* thread_2_func()
{
	while(1)
	{
		sem_wait(&mutex_thread_2);
		//Critical Section
		printf("Thread 2 read X as %d\n",sharedX);
		sleep(1);
		printf("Thread 2 wrote Y as %d\n",++sharedY);
		sem_post(&mutex_thread_1);
	}
}

main(int argc,char* argv[])
{
	pthread_t first,second;
	sem_init(&mutex_thread_1,0,0);
	sem_init(&mutex_thread_2,0,0);
	pthread_create(&first,NULL,thread_1_func,NULL);
	pthread_create(&second,NULL,thread_2_func,NULL);
	pthread_join(first,NULL);
	pthread_join(second,NULL);
	printf("Main thread completed\n");
	exit(1);
}