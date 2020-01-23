#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	fprintf(stdout,"%d|%d|%d|%d|",getuid(), getgid(), getpid(), getppid());
    if (argc > 0) {
        fprintf(stdout,"%s",argv[0]);
    }
    fprintf(stdout,"\n");
}