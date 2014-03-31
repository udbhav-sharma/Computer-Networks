#include <stdio.h>
#include <string.h>

main()
{
	char buf[255];
	while(1)
	{
		int k=read(0,buf,255);
		buf[k]='\0';
		printf("%s\n",buf);
		fflush(stdout);
	}
}