#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

sem_t mutex_thread_1,mutex_thread_2;
int fd;
int fd2;
void* thread_1_func()
{
	char buf[2];
	int i;
	fd2=open("output",O_RDWR);
	while(1)
	{
		sem_wait(&mutex_thread_1);
		//Critical section
		
		//Reading file
		printf("Thread 1 Reading\n");
		while(1)
		{
			i=read(fd,buf,1);
			if(i==0)
			{
				printf("File reading completed\n");
				exit(1);
			}
			
			write(fd2,buf,strlen(buf));
			if(buf[0]=='\n')
				break;
		}
		sem_post(&mutex_thread_2);
	}
}

void* thread_2_func()
{
	fd=open("inputq2.txt",O_RDWR);
	int x;
	char buf[2];
	while(1)
	{
		sem_post(&mutex_thread_1);
		sem_wait(&mutex_thread_2);
		//Critical Section
		
		//Writing on standard output
		printf("Thread 2 Wrote:\n");
		while(1)
		{
			read(fd2,buf,1);
			buf[1]='\0';
			write(1,buf,strlen(buf)+1);
			if(buf[0]=='\n')
				break;
		}

		printf("Want more?\n");
		scanf("%d",&x);
		if(x<0)
		{
			close(fd);
			return;
		}
	}
}

main(int argc, char*argv[])
{
	sem_init(&mutex_thread_1,0,0);
	sem_init(&mutex_thread_2,0,0);

	pthread_t thread_1,thread_2;
	pthread_create(&thread_1,NULL,thread_1_func,NULL);
	pthread_create(&thread_2,NULL,thread_2_func,NULL);
	
	pthread_join(thread_1,NULL);
	pthread_join(thread_2,NULL);
	
	exit(1);
}