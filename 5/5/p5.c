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
int shMemoryDetached;
int shAdressDetached;
char *address;
int semaphore;

void upd()
{
	sharedMemory = shmget(6969, 1, 0600 | IPC_CREAT);
	if (sharedMemory == -1)
	{
		printf("Couldn't set shared memory.\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Shared memory: %d\n", sharedMemory);
	}
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
	{
		printf("Address space: %u\n", address);
	}
}

void detachSharedMemory()
{
	shMemoryDetached = shmctl(sharedMemory, IPC_RMID, 0);
	shAdressDetached = shmdt(address);
	if (shMemoryDetached == -1 || shAdressDetached == -1)
	{
		printf("Could't detach shared memory.\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("Shared memory was sucessfully detached.\n");
}

void setSem(int semNum, int val)
{
	if (semctl(semaphore, semNum, SETVAL, val) == -1)
	{
		perror("Could not set semaphore. Exiting...");
		exit(EXIT_FAILURE);
	}
}

void createSemaphore(key_t key)
{
	semaphore = semget(key, 4, IPC_CREAT | 0600);
	if (semaphore == -1)
	{
		perror("Could not create semaphore.");
		exit(EXIT_FAILURE);
	}
}

void deleteSemaphore(void)
{
	int del_check;
	del_check = semctl(semaphore, 0, IPC_RMID);
	if (del_check == -1)
	{
		perror("Couldn't delete semaphore.");
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
		printf("%d", errno);
		exit(EXIT_FAILURE);
	}
}

int main()
{
	FILE *fp;
	upd();
	upa();
	createSemaphore(6969);

	setSem(0, 1);
	setSem(1, 0);
	setSem(2, 1);
	setSem(3, 0);

	fp = fopen("data.txt", "r");

	while (!feof(fp))
	{
		p(0); // 1. sem 0 = 0
		address[0] = fgetc(fp); // 2. put first letter 
		v(1); 
	}

	address[0] = EOF;

	setSem(2, 0);

	fclose(fp);

	p(3);

	deleteSemaphore();
	detachSharedMemory();
	exit(EXIT_SUCCESS);
}


