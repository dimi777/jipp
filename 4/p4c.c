#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/errno.h>

int semaphore;

void getSemaphore(key_t key) {
    semaphore = semget(key, 1, IPC_CREAT | 0600);
    if (semaphore == -1) {
        perror("Could not get the semaphore. Exiting...");
        exit(EXIT_FAILURE);
    }
}

void v() {
    int sem_change;
    struct sembuf buffor_sem;
    buffor_sem.sem_num = 0;
    buffor_sem.sem_op = 1;
    buffor_sem.sem_flg = SEM_UNDO;
    sem_change = semop(semaphore, & buffor_sem, 1);
    if (sem_change == -1 && errno == 4) {
        v();
        return;
    }
    if (sem_change == -1) {
        perror("semop error");
        exit(EXIT_FAILURE);
    }

}

void p() {
    int sem_change;
    struct sembuf buffor_sem;
    buffor_sem.sem_num = 0;
    buffor_sem.sem_op = -1;
    buffor_sem.sem_flg = SEM_UNDO;
    sem_change = semop(semaphore, & buffor_sem, 1);
    if (sem_change == -1 && errno == 4) {
        p();
        return;
    }
    if (sem_change == -1) {
        perror("semop error");
        exit(EXIT_FAILURE);
    }
}

void task(int num) {
    fprintf(stdout, "PID=%d task №%d\n", getpid(), num);
    sleep(1);
}

int main(int argc, char * argv[]) {
    int n, i;
    key_t key;
    if (argc != 2) {
        printf("Too few arguments. Exiting...");
        exit(EXIT_FAILURE);
    } else {
        key = atoi(argv[0]);
        n = atoi(argv[1]);
    }
    getSemaphore(key);
    for (i = 0; i < n; i++) {
        printf("PID=%d Before critical section\n", getpid());
        p();
        printf("PID=%d CRITICAL SECTION\n", getpid());
        task(i + 1);
        v();
        printf("PID=%d After critical section\n", getpid());
    }
    exit(0);
}