#include <stdio.h>
#include <string.h>

main()
{
	char buf[255];
	int portno=12034;
	printf("127.0.0.1\n");
	fflush(stdout);
	printf("%d\n",portno);
	fflush(stdout);
	while(1)
	{
		int k=read(0,buf,255);
		buf[k]='\0';
		printf("%s\n",buf);
		fflush(stdout);
	}
}