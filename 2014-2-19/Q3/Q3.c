/* 
 *eg input :: 
 *$./a.out ls wc wc
 *$./a.out ls wc
 */

#include <unistd.h>

void run_proc (int in, int out, char *cmd)
{
	if (fork () == 0)
	{
		if (in != 0)
		{
			dup2 (in, 0);
			close (in);
		}
		if (out != 1)
		{
			dup2 (out, 1);
			close (out);
		}
		execlp (cmd, cmd, NULL);
	}
}

void start_exec (int n, char **cmd)
{
	int i;
	int in, fd [2];
	//first process stdin se input lega
	in = 0;
	//all processes are executed except the last one
	for (i = 1; i < n ; ++i)
	{
		pipe (fd);
		run_proc (in, fd [1], cmd[i]);
		close (fd [1]);
		//saving the read end of the pipe
		in = fd [0];
	}
	if (in != 0)
		dup2 (in, 0);
	//executing the last process
	execlp (cmd[i], cmd[i], NULL);
}

int main (int argc, char **argv)
{
	start_exec(argc-1, argv);
}
