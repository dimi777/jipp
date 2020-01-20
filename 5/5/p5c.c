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

int sharedMemory;
char *address;
int semaphore;

void createSemaphore(key_t key)
{
	semaphore = semget(key, 4, IPC_CREAT | 0600);
	if (semaphore == -1)
	{
		perror("Couldn't create semaphore. Exiting...");
		exit(EXIT_FAILURE);
	}
}

void v(int i)
{
	int sem_change;
	struct sembuf buffor_sem;
	buffor_sem.sem_num = i;
	buffor_sem.sem_op = 1;
	buffor_sem.sem_flg = 0;
	sem_change = semop(semaphore, &buffor_sem, 1);
	if (sem_change == -1 && errno == 4)
	{
		v(i);
		return;
	}
	if (sem_change == -1)
	{
		perror("semop error");
		exit(EXIT_FAILURE);
	}
}

void p(int i)
{
	int sem_change;
	struct sembuf buffor_sem;
	buffor_sem.sem_num = i;
	buffor_sem.sem_op = -1;
	buffor_sem.sem_flg = 0;
	sem_change = semop(semaphore, &buffor_sem, 1);
	if (sem_change == -1 && errno == 4)
	{
		p(i);
		return;
	}
	if (sem_change == -1)
	{
		perror("semop error");
		exit(EXIT_FAILURE);
	}
}

void upd()
{
	sharedMemory = shmget(6969, 0, 0600 | IPC_CREAT);
	if (sharedMemory == -1)
	{
		printf("Couldn't set shared memory.\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("Shared memory: %d\n", sharedMemory);
}

void upa()
{
	address = shmat(sharedMemory, 0, 0);
	if (*address == -1)
	{
		printf("Could not set address space.\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("Address space: %u\n", address);
}

int main()
{
	FILE *fp;
	union semun {
		int val;
		struct semid_ds *buf;
		ushort *array;
	} arg;
	upd();
	upa();
	createSemaphore(6969);
	fp = fopen("received.txt", "w");
	while (semctl(semaphore, 2, GETVAL, arg) == 1) // is set to 1
	{
		p(1);
		if (address[0] != EOF)
		{
			fputc(address[0], fp);
		}
		v(0); 
	}
	if (semctl(semaphore, 3, SETVAL, 1) == -1)
	{
		printf("Couldn't set semaphore.");
	}
	fclose(fp);
	exit(EXIT_SUCCESS);
}


