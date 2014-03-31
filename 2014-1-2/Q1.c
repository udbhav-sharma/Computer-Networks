#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#define clients 4

main()
{
	int i,j,k;
	for(i=0;i<clients;i++)
	{
		char pipename[40]="file_client";
		char pipename2[10];
		j=i;
		k=0;
		while(j>0)
		{
			j=j/10;
			k++;
		}
		j=i;
		pipename2[k]='\0';
		while(k>0)
		{
			pipename2[--k]=j%10+'0';
			j/=10;
		}
		strcat(pipename,pipename2);
		printf("%s\n",pipename);
		int m=mkfifo(pipename,0666);
		if(m==-1)
		{
			printf("Error in creating %s\n",pipename);
			exit(0);
		}
	}
	exit(1);
}