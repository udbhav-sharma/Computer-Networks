#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main()
{
	char buf[100];
	while(1)
	{
		read(0,buf,99);
		write(1,buf,strlen(buf)+1);
	}
	exit(1);
}