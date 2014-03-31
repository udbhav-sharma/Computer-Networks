#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
main()
{
	char *output=(char *)malloc(100*sizeof(char));
	printf("Process 2 starts reading.\n");
	read(0,output,10);
	printf("%s\n",output);
	//dup2(STDOUT_FILENO,0);
	//read(0,output,10);
	printf("%s\n",output);
	printf("Process 2 reading completed.\n");
	exit(1);
}