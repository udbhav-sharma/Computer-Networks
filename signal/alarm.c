#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void ding(int sig)
{
	printf("Ding Dong!!!!\n");
	(void)signal(SIGALRM,SIG_DFL);
}

main()
{
	pid_t pid;
	pid=fork();
	if(pid>0)
	{
		(void)signal(SIGALRM,ding);
		printf("Waiting for signal....\n");
		pause();
		printf("Done\n");
	}
	else if(pid==0)
	{
		sleep(5);
		kill(getppid(),SIGALRM);
	}
	else
	{
		printf("Error in creating Child process.\n");
		exit(-1);
	}
	exit(1);
}