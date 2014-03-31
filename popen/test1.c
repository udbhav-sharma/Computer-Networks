#include <stdio.h>
#include <string.h>

main()
{
	int k;
	char buf[255];
	char *cmd="./test2";
	FILE* fd=popen(cmd,"r");
	if(fd==NULL)
		return;
	while(1)
	{
		fscanf(fd,"%s",buf);
    	write(1,"Client read:\t",strlen("Client read:\t"));
    	write(1,buf,strlen(buf));
    	write(1,"\n",1);
	}
}