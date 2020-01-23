#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <fcntl.h>

int array_lenght(char array[]);

int main(int argc, char* argv[])
{
	int k,n,m,i;
	int des[2];
	char c;
	char polecenie[50];
	char buffor[12];
	char buffor2[12];
	FILE *fp;
	if(argc!=3){
		printf("Zla liczba argumentow!!!\n");
		exit(EXIT_FAILURE);
	}
	if(array_lenght(argv[1])>10){
		printf("Argument przekracza inta!!\n");
		exit(EXIT_FAILURE);
	}
	if(array_lenght(argv[2])>10){
		printf("Argument przekracza inta!!\n");
		exit(EXIT_FAILURE);
	}
	n = atoi(argv[1]);
	k = atoi(argv[2]);
	
	m=atoi(getenv("MAX"));
	if(n+k>m){
		printf("Nie moge utworzyc tyle procesow\n");
		printf("Moge utworzyc %d procesow\n",m-1);
		exit(EXIT_FAILURE);
	}
	if(pipe(des)){
		printf("pipe error\n");
		exit(EXIT_FAILURE);
	}
	fcntl(des[0],F_SETFD,O_NONBLOCK|O_SYNC);
	sprintf(polecenie,"rm producent/p*");
	system(polecenie);
	sprintf(polecenie,"rm konsument/k*");
	system(polecenie);
	for(i=0;i<n;i++){
		switch(fork()){
		case -1:
			printf("fork error");
			exit(EXIT_FAILURE);
			break;
		case 0:
			close(des[0]);
			srand(getpid());
			m=10000;
			sprintf(polecenie,"producent/p%d.txt",getpid());
			fp=fopen(polecenie,"w");
			for(i=0;i<m;i++){
				c=rand()%26+65;
				write(des[1],&c,1);
				fprintf(fp,"%c",c);
			}
			sleep(1);
			close(des[1]);
			fclose(fp);
			exit(EXIT_SUCCESS);
			break;
		}
	}
	for(i=0;i<k;i++){
		switch(fork()){
		case -1:
			printf("fork error\n");
			exit(EXIT_FAILURE);
			break;
		case 0:
			close(des[1]);
			sprintf(polecenie,"konsument/k%d.txt",getpid());
			fp=fopen(polecenie,"w");
			while(1){
				n=read(des[0],&c,1);
				if(n==0){
					printf("KONIEC\n");
					close(des[0]);
					fclose(fp);
					exit(EXIT_SUCCESS);
				} else if(n!=-1) fprintf(fp,"%c",c);
			}
			break;
		}
	}
	close(des[1]);
	close(des[0]);
	for(i=0;i<k+n;i++) wait(NULL);
	exit(EXIT_SUCCESS);
}

int array_lenght(char array[]){
	int i=0;
	while(array[i]!='\0') i++;
	return i;
}
