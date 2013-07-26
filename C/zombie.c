#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    while(1)
    {
    	pid_t chi = fork();

    	if(chi == 0)
    	{
    		execl("/bin/bash","bash","-c","ls", NULL);
    	}

    	sleep(2);
    }

    return 0;
}
