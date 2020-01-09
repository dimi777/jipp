#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/errno.h>
#include <sys/wait.h>

int semafor;

void createSemaphor(key_t key) {
    semafor = semget(key, 5, IPC_CREAT | 0600);
    if (semafor == -1) {
        perror("Could not create semaphor\n");
        exit(-2);
    }
}

void deleteSemaphor() {
    int del_check;
    del_check = semctl(semafor, 0, IPC_RMID);
    if (del_check == -1) {
        perror("Could not delete semaphor\n");
        exit(1);
    }
}

void checkFile() {
    FILE * fp = fopen("spr1.txt", "w");
    if (fp == NULL) {
        printf("couldn't open file. exiting...");
        exit(5);
    }
    fclose(fp);
}

int main() {
    checkFile();
    int i, w, x, sem_ctl;
    int children_ids[3];
    char keyToString[12], cmd[50], children[3][5] = {"p3p1","p3p2","p3p3"};
    ushort semvals[5] = {0, 0, 0, 0, 0};
    srand(getpid());
    key_t key = ftok(".", rand() % 123);
    if (key == -1) {
        printf("Could not create the key. Exiting...\n");
        exit(-1);
    }
    sprintf(keyToString, "%d", key);
    printf("Semaphores creation...\n");
    createSemaphor(key);
    printf("Semaphores created.\nSetting semaphores...\n");
    sem_ctl = semctl(semafor, 0, SETALL, semvals);
    if (sem_ctl == -1) {
        perror("semctl error");
        deleteSemaphor();
        exit(3);
    }
    printf("Semaphores are set\nCreate 3 children processes...\n");
    for (i = 0; i < 3; i++) {
        switch (children_ids[i] = fork()) {
            case -1:
                perror("Fork error");
                exit(-3);
            case 0:
                if (execl(children[i], keyToString, NULL)) {
                    perror("Exec error");
                    sleep(1);
                    exit(1);
                }
        }
    }
    printf("Children processes created\nWaiting for children processes to finish...\n");
    for (i = 0; i < 3; i++) {
        w = wait( & x);
        if (x != 0) {
            printf("Child process return an error, killing all other children\n");
            for (i = 0; i < 3; i++) {
                if (children_ids[i] != w) {
                    sprintf(cmd, "kill -9 %d", children_ids[i]);
                    system(cmd);
                }
            }
        } else {
            fprintf(stdout, "Proces %d zakonczyl sie z kodem powrotu %d\n", w, x);
        }
    }
    printf("Processes are finished, now deleting semaphores...\n");
    deleteSemaphor();
    printf("Semaphores deleted\n");
    exit(0);
}
