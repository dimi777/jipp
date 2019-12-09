#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>


int main()
{
	int id, main_process_id;

	fprintf(stdout," UID| GID|  PID| PPID|MESSAGE\n");
	fprintf(stdout,"%d|%d|%d|%d|Main process\n", getuid(), getgid(), getpid(), getppid());
	main_process_id = getpid();
	for(int k = 0; k < 3; k++) {
		id = fork();
	}

	switch (id) { 
		case -1:
			perror("fork error");
			exit(0);
		case 0:
			execl("./print_process_info", "im a child process", NULL);
			//fprintf(stdout,"%d|%d|%d|%d|child\n",getuid(), getgid(), getpid(), getppid());
			//break;
		default:
			fprintf(stdout,"%d|%d|%d|%d|im parent of %d\n", getuid(), getgid(), getpid(), getppid(), id);
			char pid[20];
			sprintf(pid, "pstree -p -c %d", getpid());
			system(pid);
			wait(NULL);
	}

	//fprintf(stdout,"%d|%d|%d|%d|exit\n", getuid(), getgid(), getpid(), getppid());
	exit(1);
}

