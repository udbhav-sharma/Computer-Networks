#include <stdio.h>
#include <string.h>

void uppercase(char *buf)
{
    int i;
    for(i=0;i<strlen(buf);i++)
        if(buf[i]<='z' && buf[i]>='a')
            buf[i]=(buf[i]-'a'+'A');
}
main()
{
	char buf[255];
	while(1)
	{
		int k=read(0,buf,255);
		buf[k]='\0';
		uppercase(buf);
		write(1,buf,strlen(buf)+1);
	}
}