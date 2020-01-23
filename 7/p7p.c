#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>

int array_lenght(char array[]);

int main(int argc, char* argv[])
{
	int n,m,i;
	char polecenie[55];
	char buffor[12];
	FILE *fp;
	if(argc!=2){
		printf("Zla liczba argumentow!!!\n");
		exit(EXIT_FAILURE);
	}
	if(array_lenght(argv[1])>10){
		printf("Argument przekracza inta!!\n");
		exit(EXIT_FAILURE);
	}
	n =  atoi(argv[1]);
	
	m=atoi(getenv("MAX"));
	if(n>=m){
		printf("Nie moge utworzyc tyle procesow\n");
		printf("Moge utworzyc %d procesow\n",m-1);
		exit(EXIT_FAILURE);
	}
	if(mkfifo("potok",0600)){
		if(errno!=EEXIST){
			printf("mkfifo error");
			exit(EXIT_FAILURE);
		}
	}
	sprintf(polecenie,"rm producent/p*");
	system(polecenie);
	for(i=0;i<n;i++){
		switch(fork()){
		case -1:
			printf("fork error");
			exit(EXIT_FAILURE);
			break;
		case 0:
			if(execl("p","p",NULL)){
				printf("execl error");
				exit(EXIT_FAILURE);
			}
			break;
		}
	}
	//for(i=0;i<n;i++) wait(NULL);
	//sprintf(polecenie,"rm potok");
	//system(polecenie);
	exit(EXIT_SUCCESS);
}

int array_lenght(char array[]){
	int i=0;
	while(array[i]!='\0') i++;
	return i;
}
