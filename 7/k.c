#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>

void mkfifo_read();

int main(int argc, char* argv[])
{
	mkfifo_read();
	exit(EXIT_SUCCESS);
}

void mkfifo_read(){
	int i,fd,check;
	FILE *fp;
	char c;
	char buf[30];
	sprintf(buf,"konsument/k%d.txt",getpid());
	fp=fopen(buf,"w");
	fd=open("potok",O_RDONLY|O_NONBLOCK|O_TRUNC);
	if(fd==-1){
		printf("open error");
		exit(EXIT_FAILURE);
	}
	while((check=read(fd,&c,1))!=0){
		if (check > 0) fprintf(fp,"%c",c);
		else if(check == -1 && errno != 11){
			printf("cos nie tak\n");
			perror("Blad read");
		}
	}
	printf("KONIEC\n");
	close(fd);
	fclose(fp);
	exit(EXIT_SUCCESS);
}
