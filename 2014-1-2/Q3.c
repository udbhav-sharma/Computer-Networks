#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
main()
{
	int fd=open("Q3.txt",O_CREAT|O_RDWR);
	dup2(fd,0);
	int pid=fork();
	if(pid>0)
	{
		wait(NULL);
		char *buf=(char *)malloc(100*sizeof(char));
		read(0,buf,10);
		printf("%s\n",buf);
		/*while(strcmp(buf,"-1")!=0)
		{
			printf("%s\n",buf);
			scanf("%s",buf);
		}*/
		printf("Parent Process ends.\n");
	}
	else if(pid==0)
	{
		char *cmd[1];
		cmd[0]=(char *)malloc(10*sizeof(char));
		cmd[0]="./Q3p2";
		execvp(cmd[0],cmd);
	}
	else
	{
		printf("Error in creating child process.\n");
		exit(0);
	}

	exit(1);
}