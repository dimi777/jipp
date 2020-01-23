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

#define MYCOND "/sdsfsdf0"
#define MYCOND1 "/sdsfsdf1"
#define MYCOND2 "/sdsfsdf2"
#define MYCOND3 "/sdsfsdf3"

int i, w, x;
pid_t pid;
pthread_cond_t *cond;
pthread_cond_t *cond1;
pthread_cond_t *cond2;
pthread_cond_t *cond3;
int cond_id;
int cond_id1;
int cond_id2;
int cond_id3;
shared_mutex_t mutex;
shared_mutex_t mutex1;
shared_mutex_t mutex2;
shared_mutex_t mutex3;

int mode = S_IRWXU | S_IRWXG;

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
    mutex = shared_mutex_init("/scip5", 0600);
    if (mutex.ptr == NULL)
    {
        return -1;
    }
    if (mutex.created)
    {
        printf("The mutex with name \"/b%d\" was created\n", 0);
    }
    /************************************************************** */
    mutex1 = shared_mutex_init("/scip6", 0600);
    if (mutex1.ptr == NULL)
    {
        return -1;
    }
    if (mutex1.created)
    {
        printf("The mutex with name \"/b%d\" was created\n", 1);
    }
    /*********************************************************** */
     mutex2 = shared_mutex_init("/scip7", 0600);
    if (mutex2.ptr == NULL)
    {
        return -1;
    }
    if (mutex2.created)
    {
        printf("The mutex with name \"/b%d\" was created\n", 2);
    }
    /*********************************************************** */
     mutex3 = shared_mutex_init("/scip8", 0600);
    if (mutex3.ptr == NULL)
    {
        return -1;
    }
    if (mutex3.created)
    {
        printf("The mutex with name \"/b%d\" was created\n", 3);
    }
    /* cond */
    cond_id = shm_open(MYCOND, O_RDWR, S_IRWXU);
    if (cond_id < 0)
    {
        perror("shm_open failed");
        exit(-1);
    }
    cond = (pthread_cond_t *)mmap(NULL, sizeof(pthread_cond_t), PROT_READ | PROT_WRITE, MAP_SHARED, cond_id, 0);
    if (cond == MAP_FAILED)
    {
        perror("ftruncate failed with " MYCOND);
        return -1;
    }
    pthread_condattr_t cattr;
    pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(cond, &cattr);
    pthread_condattr_destroy(&cattr);
    /********************************************************************8 */
    cond_id1 = shm_open(MYCOND1, O_RDWR, S_IRWXU);
    if (cond_id1 < 0)
    {
        perror("shm_open failed");
        exit(-1);
    }
    cond1 = (pthread_cond_t *)mmap(NULL, sizeof(pthread_cond_t), PROT_READ | PROT_WRITE, MAP_SHARED, cond_id1, 0);
    if (cond1 == MAP_FAILED)
    {
        perror("ftruncate failed with " MYCOND1);
        return -1;
    }
    pthread_condattr_t cattr1;
    pthread_condattr_setpshared(&cattr1, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(cond1, &cattr1);
    pthread_condattr_destroy(&cattr1);

    /****************************************** */

    cond_id2 = shm_open(MYCOND2, O_RDWR, S_IRWXU);
    if (cond_id2 < 0)
    {
        perror("shm_open failed");
        exit(-1);
    }
    cond2 = (pthread_cond_t *)mmap(NULL, sizeof(pthread_cond_t), PROT_READ | PROT_WRITE, MAP_SHARED, cond_id2, 0);
    if (cond2 == MAP_FAILED)
    {
        perror("ftruncate failed with " MYCOND2);
        return -1;
    }
    pthread_condattr_t cattr2;
    pthread_condattr_setpshared(&cattr2, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(cond2, &cattr2);
    pthread_condattr_destroy(&cattr2);

    /************************************* */

    cond_id3 = shm_open(MYCOND3, O_RDWR, S_IRWXU);
    if (cond_id3 < 0)
    {
        perror("shm_open failed");
        exit(-1);
    }
    cond3 = (pthread_cond_t *)mmap(NULL, sizeof(pthread_cond_t), PROT_READ | PROT_WRITE, MAP_SHARED, cond_id3, 0);
    if (cond3 == MAP_FAILED)
    {
        perror("ftruncate failed with " MYCOND1);
        return -1;
    }
    pthread_condattr_t cattr3;
    pthread_condattr_setpshared(&cattr3, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(cond3, &cattr3);
    pthread_condattr_destroy(&cattr3);

    /*********************************** */
    sleep(20);
    pthread_mutex_lock(mutex2.ptr);
    fprintf(stdout, "p1 waitin for cond 2\n");
    pthread_cond_wait(cond2, mutex2.ptr);
    task(11);
    task(12);
    pthread_mutex_unlock(mutex2.ptr);

    pthread_mutex_lock(mutex3.ptr);
    fprintf(stdout, "p1 signal cond 3\n");
    if (pthread_cond_signal(cond3))
    {
        return 1;
    }
    pthread_mutex_unlock(mutex3.ptr);

    return 0;
}