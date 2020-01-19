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
int odlaczenie1;
int odlaczenie2;
char *adres;
int semafor;

void create_sem(key_t key);
void delete_sem(void);
void sem_up(int i);
void sem_down(int i);

void upd()
  {
    pamiec=shmget(6969,1,0600|IPC_CREAT);
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

void odlacz_pamiec()
  {
    odlaczenie1=shmctl(pamiec,IPC_RMID,0);
    odlaczenie2=shmdt(adres);
    if (odlaczenie1==-1 || odlaczenie2==-1)
      {
        printf("Problemy z odlaczeniem pamieci dzielonej.\n");
        exit(EXIT_FAILURE);
      }
    else printf("Pamiec dzielona zostala odlaczona.\n");
  }

int main()
  {
	FILE *fp;
	char c='a';
    	upd();
    	upa();
	create_sem(6969);
	if(semctl(semafor,0,SETVAL,1)==-1){
		perror("Nie udalo sie ustawic semafora");
		exit(EXIT_FAILURE);
	}
	if(semctl(semafor,1,SETVAL,0)==-1){
		perror("Nie udalo sie ustawic semafora");
		exit(EXIT_FAILURE);
	}
	if(semctl(semafor,2,SETVAL,1)==-1){
		perror("Nie udalo sie ustawic semafora");
		exit(EXIT_FAILURE);
	}
	if(semctl(semafor,3,SETVAL,0)==-1){
		perror("Nie udalo sie ustawic semafora");
		exit(EXIT_FAILURE);
	}
	fp=fopen("plik1.txt","r");
    	while(!feof(fp)){
		sem_down(0);
		adres[0] = fgetc(fp);
		sem_up(1);
	}
	adres[0] = 1;
	semctl(semafor,2,SETVAL,0);
	fclose(fp);
	sem_down(3);
	delete_sem();
    	odlacz_pamiec();
    	exit(EXIT_SUCCESS);
  }

void create_sem(key_t key){
	semafor = semget(key,4,IPC_CREAT|0600);
	if(semafor == -1){
		perror("Nie udalo sie stworzyc semafora");
		exit(EXIT_FAILURE);
	}
}

void delete_sem(void){
	int del_check;
	del_check=semctl(semafor,0,IPC_RMID);
	if(del_check==-1){
		perror("Ni eudalo sie usunac semafora");
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
		printf("%d",errno);
		exit(EXIT_FAILURE);
	}
}
