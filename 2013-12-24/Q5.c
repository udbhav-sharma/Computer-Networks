#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
main()
{
	int file1,file2;
	file1=mkfifo("file_server",0666);
	if(file1==-1)
	{
		printf("Error in creating FIIFO file_Server");
		exit(-1);
	}
	file2=mkfifo("file_client",0666);
	if(file2==-1)
	{
		printf("Error in creating FIIFO file_Client");
		exit(-1);
	}
	exit(1);
}
