#include <stdio.h>
#include <string.h>

void lowercase(char *buf)
{
    int i;
    for(i=0;i<strlen(buf);i++)
        if(buf[i]<='Z' && buf[i]>='A')
            buf[i]=(buf[i]-'A'+'a');
}

main()
{
	char buf[255];
	while(1)
	{
		int k=read(0,buf,255);
		buf[k]='\0';
		lowercase(buf);
		write(1,buf,strlen(buf)+1);
	}
}