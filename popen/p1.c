#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

main()
{
	char *cmd="./p2";
	FILE* fd=popen(cmd,"R");
	char buf[100];
	while(1)
	{
		read(fd,buf,99);
		write(1,"Process 1 wrote: ",20);
		write(1,buf,strlen(buf)+1);
		write(1,"\n",2);
	}
	exit(1);
}