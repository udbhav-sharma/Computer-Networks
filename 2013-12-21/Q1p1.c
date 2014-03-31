#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
main()
{
	printf("Process 1 Starts\n");
	int pid=fork();
	if(pid>0)
	{
		printf("Process 1 Waiting for Child\n");
		waitpid(pid,NULL,0);
		printf("Process 1 ends\n");
	}
	else if(pid==0)
	{
		printf("Process 2 getting called\n");
		char* argv[1];
		argv[0]=(char*)malloc(15 * sizeof(char));
		argv[0]="./Q1p2";
		execvp(argv[0],argv);
	}
	else
	{
		printf("Fork error in Q1p1.c\n");
	}
	exit(1);
}