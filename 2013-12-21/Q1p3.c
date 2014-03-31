#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
main()
{
	int i;
	printf("Process 3 Starts\n");
	printf("For ending process 3 press any number");
	scanf("%d",&i);
	printf("Process 3 ends\n");
	exit(1);
}