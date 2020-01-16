#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    printf("UID=%i GID=%i PID=%i PPID=%i PGID=%i\n", getuid(), getgid(), getpid(), getppid(), getpgid(0));

   exit(0);
}

