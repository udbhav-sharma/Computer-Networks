#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
main()
{
	printf("Process 2 Starts\n");
	int pid=fork();
	if(pid>0)
	{
		printf("Process 2 Waiting for Child\n");
		waitpid(pid,NULL,NULL);
		printf("Process 2 ends\n");
	}
	else if(pid==0)
	{
		printf("Process 3 getting called\n");
		char* argv[1];
		argv[0]=(char*)malloc(15 * sizeof(char));
		argv[0]="./Q1p3";
		execvp(argv[0],argv);
	}
	else
	{
		printf("Fork error in Q1p2.c\n");
	}
	exit(1);
}