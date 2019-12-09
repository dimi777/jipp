#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>


int main()
{
	int id;

	fprintf(stdout," UID| GID|  PID| PPID|MESSAGE\n");
	fprintf(stdout,"%d|%d|%d|%d|Main process\n", getuid(), getgid(), getpid(), getppid());

	for(int k = 0; k < 4; k++) {
		id = fork();
	}

	switch (id) { 
		case -1:
			perror("fork error");
			exit(0);
		case 0:
			//execl("./print_process_info", "im a child process", NULL);
			fprintf(stdout,"%d|%d|%d|%d|child\n",getuid(), getgid(), getpid(), getppid());
			break;
		default:
			fprintf(stdout,"%d|%d|%d|%d|im parent of %d\n", getuid(), getgid(), getpid(), getppid(), id);
			wait(NULL);
	}

	fprintf(stdout,"%d|%d|%d|%d|exit\n", getuid(), getgid(), getpid(), getppid());
	exit(1);
}

