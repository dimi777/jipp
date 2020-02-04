/*
 * gcc mutex.c -o mutex -lrt
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "shared_mutex.h"

int i, w, x;
pid_t pid;

shared_mutex_t mutex[4];
pthread_cond_t *cond[4];
int cond_id[4];

int mode = S_IRWXU | S_IRWXG;

int createMutex()
{
    for (i = 0; i < 4; i++)
    {
        char *mutex_name;
        sprintf(mutex_name, "/mutex%d", i);
        mutex[i] = shared_mutex_init(mutex_name, 0600);
        if (mutex[i].ptr == NULL)
        {
            return -1;
        }
        if (mutex[i].created)
        {
            printf("The mutex with name \"/mutex%d\" was created\n", i);
        }
    }
    return 0;
}

int createCond()
{
    for (i = 0; i < 4; i++)
    {
        char *cond_name;
        sprintf(cond_name, "/cond%d", i);
        cond_id[i] = shm_open(cond_name, O_RDWR, S_IRWXU);
        if (cond_id[i] < 0)
        {
            perror("shm_open failed");
            exit(-1);
        }
        cond[i] = (pthread_cond_t *)mmap(NULL, sizeof(pthread_cond_t), PROT_READ | PROT_WRITE, MAP_SHARED, cond_id[i], 0);
        if (cond[i] == MAP_FAILED)
        {
            return -1;
        }
        pthread_condattr_t cattr;
        pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
        pthread_cond_init(cond[i], &cattr);
        pthread_condattr_destroy(&cattr);
    }
    return 0;
}

void task(int num)
{
    sleep(1);
    fprintf(stdout, "----Sekcja t%d procesu o PID=%d\n", num, getpid());
    FILE *fp = fopen("spr1.txt", "a");
    fprintf(fp, "Sekcja t%d procesu o PID=%d\n", num, getpid());
    fclose(fp);
}

int main(int argc, char *argv[])
{
    createMutex();
    createCond();
    /*********************************** */
    sleep(5);
    pthread_mutex_lock(mutex[2].ptr);
    fprintf(stdout, "p1 waitin for cond 2\n");
    pthread_cond_wait(cond[2], mutex[2].ptr);
    task(11);
    task(12);
    pthread_mutex_unlock(mutex[2].ptr);

    pthread_mutex_lock(mutex[3].ptr);
    fprintf(stdout, "p1 signal cond 3\n");
    if (pthread_cond_signal(cond[3]))
    {
        return 1;
    }
    pthread_mutex_unlock(mutex[3].ptr);

    return 0;
}