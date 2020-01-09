#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/errno.h>

int semafor;

void get_sem(key_t key);
void sem_up(int semnum);
void sem_down(int semnum);
void task(int num);

int main(int argc, char** argv)
  {
	key_t key;
	if(argc!=1){
		printf("Za malo argumentow przekazanych do funkcji!!!");
		exit(1);
	}else{
		key = atoi(argv[0]);
	}
	get_sem(key);
	sem_down(2);
	task(1);
	task(2);
	sem_up(3);
	sleep(5);
	exit(0);
  }

void get_sem(key_t key){
	semafor = semget(key,3,IPC_CREAT);
	if(semafor == -1){
		perror("Nie udalo sie pobrac semafora");
		exit(-2);
	}
}

void sem_up(int semnum){
	int sem_change;
	struct sembuf buffor_sem;
	buffor_sem.sem_num=semnum;
	buffor_sem.sem_op=1;
	buffor_sem.sem_flg=SEM_UNDO;
	sem_change=semop(semafor,&buffor_sem,1);
	if(sem_change==-1 && errno==4){
		sem_up(semnum);
		return ;
	}
	if(sem_change==-1){
		perror("semop error");
		exit(4);
	}

}

void sem_down(int semnum){
	int sem_change;
	struct sembuf buffor_sem;
	buffor_sem.sem_num=semnum;
	buffor_sem.sem_op=-1;
	buffor_sem.sem_flg=SEM_UNDO;
	sem_change=semop(semafor,&buffor_sem,1);
	if(sem_change==-1 && errno==4){
		sem_down(semnum);
		return ;
	}
	if(sem_change==-1){
		perror("semop error");
		exit(4);
	}
}

void task(int num){
	fprintf(stdout,"Sekcja t1%d procesu o PID=%d\n",num,getpid());
	sleep(1);
	FILE *fp = fopen("spr1.txt","a");
	fprintf(fp,"Sekcja t1%d procesu o PID=%d\n",num,getpid());
	fclose(fp);
}
