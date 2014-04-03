#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>

int fd[2];

int main()
{
	int len, flags;
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
		close(fd[1]);
		
		int rc = ioctl(fd[0], FIONBIO, (char *)&"on");
		
		printf("Parent is ready!\n");
		while(1)
		{
			memset(buf, '\0', 200);
			//this read is now non-blocking
			read(fd[0],buf,190);
			
			printf("%d\n",(int)strlen(buf));
			if (strlen(buf) == 0)
				continue;
			printf("Child send : %s\n",buf);	
			if (strcmp(buf,"Hello from child") == 0)
				break;
		}
	}
}
