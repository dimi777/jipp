#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>

void mkfifo_write(int n);

int main(int argc, char* argv[])
{
	int n;
	srand(getpid());
	n=1000;
	mkfifo_write(n);
	exit(EXIT_SUCCESS);
}

void mkfifo_write(int n){
	int fd;
	FILE *fp;
	int i;
	char buf[30];
	char c;	
	sprintf(buf,"producent/p%d.txt",getpid());
	fp=fopen(buf,"w");
	fd= open("potok",O_WRONLY|O_TRUNC);
	if(fd==-1){
		printf("open error");
		exit(EXIT_FAILURE);
	}
	//sleep(1);
	for(i=0;i<n;i++){
		c=rand()%26+65;
		write(fd,&c,1);
		fprintf(fp,"%c",c);
	}
	close(fd);
	fclose(fp);
}
