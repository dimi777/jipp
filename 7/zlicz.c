#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	FILE *fp;
	int p,k;
	char buf[12];
	char polecenie[50];
	sprintf(polecenie,"cat producent/p*|wc -c");
	fp=popen(polecenie,"r");
	if(fp==NULL){
		printf("popen error\n");
		exit(EXIT_FAILURE);
	}
	while(fgets(buf,10,fp)!=NULL);
	fclose(fp);
	p=atoi(buf);
	sprintf(polecenie,"cat konsument/k*|wc -c");
	fp=popen(polecenie,"r");
	if(fp==NULL){
		printf("popen error\n");
		exit(EXIT_FAILURE);
	}
	while(fgets(buf,10,fp)!=NULL);
	fclose(fp);
	k=atoi(buf);
	printf("Liczba znakow w plikach producentow: %d\n",p);
	printf("Liczba znakow w plikach konsumentow: %d\n",k);
	return 0;
}
