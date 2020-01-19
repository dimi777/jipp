#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <string.h>
#include <sys/errno.h>

int pamiec;
char *adres;
int semafor;

void create_sem(key_t key);
void sem_up(int i);
void sem_down(int i);

void upd()
  {
    pamiec=shmget(6969,0,0600|IPC_CREAT);
    if (pamiec==-1) 
      {
        printf("Problemy z utworzeniem pamieci dzielonej.\n");
        exit(EXIT_FAILURE);
      }
    else printf("Pamiec dzielona zostala utworzona : %d\n",pamiec);
  }

void upa()
  {
    adres=shmat(pamiec,0,0);
    if (*adres==-1) 
      {
        printf("Problem z przydzieleniem adresu.\n");
        exit(EXIT_FAILURE);
      }    
    else printf("Przestrzen adresowa zostala przyznana : %u\n",adres);
  }


int main()
  {
	FILE *fp;
	union semun{
		int val;
		struct semid_ds *buf;
		ushort *array;
	} arg;
    	upd();
    	upa();
	create_sem(6969);
	fp=fopen("plik2.txt","w");
    	while(semctl(semafor,2,GETVAL,arg)==1){
		sem_down(1);
		if(adres[0] != EOF && adres[0] != 1)fputc(adres[0],fp);
		sem_up(0);
	}
	if(semctl(semafor,3,SETVAL,1)==-1){
		printf("Nie udalo sie ustawic semafora");
	}
	fclose(fp);
    	exit(EXIT_SUCCESS);
  }

void create_sem(key_t key){
	semafor = semget(key,4,IPC_CREAT|0600);
	if(semafor == -1){
		perror("Nie udalo sie stworzyc semafora");
		exit(EXIT_FAILURE);
	}
}

void sem_up(int i){
	int sem_change;
	struct sembuf buffor_sem;
	buffor_sem.sem_num=i;
	buffor_sem.sem_op=1;
	buffor_sem.sem_flg=0;
	sem_change=semop(semafor,&buffor_sem,1);
	if(sem_change==-1 && errno==4){
		sem_up(i);
		return;
	}
	if(sem_change==-1){
		perror("semop error");
		exit(EXIT_FAILURE);
	}
}

void sem_down(int i){
	int sem_change;
	struct sembuf buffor_sem;
	buffor_sem.sem_num=i;
	buffor_sem.sem_op=-1;
	buffor_sem.sem_flg=0;
	sem_change=semop(semafor,&buffor_sem,1);
	if(sem_change==-1 && errno==4){
		sem_down(i);
		return;
	}
	if(sem_change==-1){
		perror("semop error");
		exit(EXIT_FAILURE);
	}
}
