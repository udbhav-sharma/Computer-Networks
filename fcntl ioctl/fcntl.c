#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int fd[2];

void hand_sigIO()
{
	printf("Parent received msg\n");
	fflush(stdout);

	char buf[200];
	int k=read(fd[0],buf,190);
	
	if(k==0)
		printf("Child process ended\n");
	else
	{
		buf[k]='\0';
		printf("The Child process send : %s\n",buf);
	}
	fflush(stdout);
}

int main()
{
	int flags;
	char buf[200];
	
	pipe(fd);
	
	if (fork() == 0)
	{
		close(fd[0]);
		
		printf("Child started\n");
		fflush(stdout);

		strcpy(buf, "Hello from child");
		
		printf("child :: sending data\n");
		fflush(stdout);

		write(fd[1], buf, strlen(buf));
		
		printf("Child exiting\n");
		fflush(stdout);

		exit(1);
	}
	else
	{
		signal(SIGIO, hand_sigIO);
		close(fd[1]);

		fcntl(fd[0], F_SETOWN, (int)getpid());
		
		flags = fcntl(fd[0], F_GETFL);
		flags |= O_NONBLOCK|O_ASYNC;
		fcntl(fd[0], F_SETFL, flags);
		
		printf("Parent is ready.\n");
		wait(NULL);
	}
}
	
	
	
