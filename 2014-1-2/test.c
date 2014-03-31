#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
main()
{
	int fd=open("Q3.txt",O_RDWR);
	dup2(fd,0);
	printf("Process 1 Starts\n");
	int pid=fork();
	if(pid>0)
	{
		printf("Process 1 Waiting for Child\n");
		waitpid(pid,NULL,0);
		char output[100];
		scanf("%s",output);
		printf("%s\n",output);
		printf("Process 1 ends\n");
	}
	else if(pid==0)
	{
		printf("Process 1 child is called\n");
		char* argv[1];
		argv[0]=(char*)malloc(15 * sizeof(char));
		argv[0]="./Q3p2";
		execvp(argv[0],argv);
	}
	else
	{
		printf("Fork error in Q1p1.c\n");
	}
	exit(1);
}