#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

int main(){
	char cmd[20];
	int i, pid = getpid();
	fprintf(stdout," UID| GID|  PID| PPID|MESSAGE\n");
	fprintf(stdout,"%d|%d|%d|%d|Main process\n", getuid(), getgid(), getpid(), getppid());
	sprintf(cmd,"pstree -p %i",getpid());
	for(i = 0; i < 3; i++){
		switch(fork()){
			case -1:
				perror("Fork error.");
				exit(1);
			case 0:
				fprintf(stdout,"i = %d\n",i);
				execl("./print_process_info", "im a child process", NULL);
				//fprintf(stdout,"%d|%d|%d|%d|child i = %d\n",getuid(), getgid(), getpid(), getppid(), i);
				//break;
		}
	}
	if(pid == getpid()) {
		system(cmd);
	}
	sleep(1);
	exit(0);
}
