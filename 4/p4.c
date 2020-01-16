#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/errno.h>
#include <sys/wait.h>

int semaphore;

void createSemaphore(key_t key) {
    semaphore = semget(key, 1, IPC_CREAT | 0600);
    if (semaphore == -1) {
        perror("Could not create the semaphore\n");
        exit(EXIT_FAILURE);
    }
}

void deleteSemaphore(void) {
    int del_check;
    del_check = semctl(semaphore, 0, IPC_RMID);
    if (del_check == -1) {
        perror("Could not delete the semaphore\n");
        exit(EXIT_FAILURE);
    }
}

int array_length(char array[]) {
    int i = 0;
    while (array[i] != '\0') {
        i++;
    }
    return i;
}

int main(int argc, char * argv[]) {
    FILE * fp;
    int i, w, x, n, m, sem_ctl;
    char buffor[20], buffor2[20], polecenie[55];

    if (argc != 4) {
        printf("Not enough args\n");
        exit(EXIT_FAILURE);
    }

    if (array_length(argv[2]) > 10) {
        fprintf(stdout, "Number of children is too big. Exiting...\n");
        exit(EXIT_FAILURE);
    }

    if (array_length(argv[3]) > 10) {
        fprintf(stdout, "Number of critical section repetition too big. Exiting...\n");
        exit(EXIT_FAILURE);
    }

    n = atoi(argv[2]);
    sprintf(polecenie, "echo $(( 55393 - `ps -u dmitry | wc -l` + 2 ))");
    fp = popen(polecenie, "r");
    if (fp == NULL) {
        printf("Can't create pipe. Exiting...\n");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffor2, 10, fp) != NULL);
    
    fclose(fp);
    m = atoi(buffor2);

    if (n > m) {
        printf("Can only create %d processes. %d requested\n",m,n);
        exit(EXIT_FAILURE);
    }

    srand(getpid());
    key_t key = ftok(".", rand() % 123);

    if (key == -1) {
        printf("Cannot create the semaphore key.\n");
        exit(EXIT_FAILURE);
    }
    sprintf(buffor, "%d", key);
    createSemaphore(key);
    fprintf(stdout, "Semaphore ID: %u\n", semaphore);
    if (semctl(semaphore, 0, SETVAL, 1) == -1) {
        perror("semctl error");
        deleteSemaphore();
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "Actions:\n");
    for (i = 0; i < n; i++) {
        switch (fork()) {
        case -1:
            perror("Fork error");
            exit(EXIT_FAILURE);
        case 0:
            if (execl(argv[1], buffor, argv[3], NULL) == -1) {
                perror("Exec error");
                sleep(1);
                exit(EXIT_FAILURE);
            }
        }
    }
    for (i = 0; i < n; i++) {
        w = wait( & x);
        fprintf(stdout, "Process %d exited with code %d\n", w, x);
    }
    deleteSemaphore();
    printf("Semaphores deleted.\n");
    exit(0);
}
