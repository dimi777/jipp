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
#include <sys/wait.h>
#include <fcntl.h>
#include "shared_mutex.h"


#define MYCOND "/sdsfsdf0"
#define MYCOND1 "/sdsfsdf1"
#define MYCOND2 "/sdsfsdf2"
#define MYCOND3 "/sdsfsdf3"

shared_mutex_t mutex;
shared_mutex_t mutex1;
shared_mutex_t mutex2;
shared_mutex_t mutex3;

int i, w, x;
pid_t pid;
int children_ids[3];
char cmd[50], children[3][5] = {"p1", "p2", "p3"};
int mode = 0600;

void checkFile();

int createMutex()
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
    /*********************************************************** */
}

int main(int argc, char *argv[])
{
    char mutex_name[9], cond_name[9];
    checkFile();
    createMutex();
    if (createMutex())
    {
        printf("fail");
        return -1;
    }
    /* mutex */
    pthread_cond_t *cond;
    pthread_cond_t *cond1;
    pthread_cond_t *cond2;
    pthread_cond_t *cond3;
    int cond_id, mutex_id;
    int cond_id1, cond_id2, cond_id3;
    int mode = S_IRWXU | S_IRWXG;

    /* cond */
    cond_id = shm_open(MYCOND, O_CREAT | O_RDWR | O_TRUNC, mode);
    if (cond_id < 0)
    {
        perror("shm_open failed with " MYCOND);
        return -1;
    }
    if (ftruncate(cond_id, sizeof(pthread_cond_t)) == -1)
    {
        perror("ftruncate failed with " MYCOND);
        return -1;
    }
    cond = (pthread_cond_t *)mmap(NULL, sizeof(pthread_cond_t), PROT_READ | PROT_WRITE, MAP_SHARED, cond_id, 0);
    if (cond == MAP_FAILED)
    {
        perror("ftruncate failed with " MYCOND);
        return -1;
    }
    /* set condition shared between processes */
    pthread_condattr_t cattr;
    pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(cond, &cattr);
    pthread_condattr_destroy(&cattr);
    /***************************COND 1*************************************** */
    cond_id1 = shm_open(MYCOND1, O_CREAT | O_RDWR | O_TRUNC, mode);
    if (cond_id1 < 0)
    {
        perror("shm_open failed with " MYCOND1);
        return -1;
    }
    if (ftruncate(cond_id1, sizeof(pthread_cond_t)) == -1)
    {
        perror("ftruncate failed with " MYCOND1);
        return -1;
    }
    cond1 = (pthread_cond_t *)mmap(NULL, sizeof(pthread_cond_t), PROT_READ | PROT_WRITE, MAP_SHARED, cond_id1, 0);
    if (cond1 == MAP_FAILED)
    {
        perror("ftruncate failed with " MYCOND1);
        return -1;
    }
    /* set condition shared between processes */
    pthread_condattr_t cattr1;
    pthread_condattr_setpshared(&cattr1, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(cond, &cattr1);
    pthread_condattr_destroy(&cattr1);
    /*************************************/
      /***************************COND 2*************************************** */
    cond_id2 = shm_open(MYCOND2, O_CREAT | O_RDWR | O_TRUNC, mode);
    if (cond_id2 < 0)
    {
        perror("shm_open failed with " MYCOND2);
        return -1;
    }
    if (ftruncate(cond_id2, sizeof(pthread_cond_t)) == -1)
    {
        perror("ftruncate failed with " MYCOND2);
        return -1;
    }
    cond2 = (pthread_cond_t *)mmap(NULL, sizeof(pthread_cond_t), PROT_READ | PROT_WRITE, MAP_SHARED, cond_id2, 0);
    if (cond2 == MAP_FAILED)
    {
        perror("ftruncate failed with " MYCOND2);
        return -1;
    }
    /* set condition shared between processes */
    pthread_condattr_t cattr2;
    pthread_condattr_setpshared(&cattr2, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(cond, &cattr2);
    pthread_condattr_destroy(&cattr2);
    /*************************************/
      /***************************COND 3*************************************** */
    cond_id3 = shm_open(MYCOND3, O_CREAT | O_RDWR | O_TRUNC, mode);
    if (cond_id3 < 0)
    {
        perror("shm_open failed with " MYCOND3);
        return -1;
    }
    if (ftruncate(cond_id3, sizeof(pthread_cond_t)) == -1)
    {
        perror("ftruncate failed with " MYCOND3);
        return -1;
    }
    cond3 = (pthread_cond_t *)mmap(NULL, sizeof(pthread_cond_t), PROT_READ | PROT_WRITE, MAP_SHARED, cond_id3, 0);
    if (cond3 == MAP_FAILED)
    {
        perror("ftruncate failed with " MYCOND3);
        return -1;
    }
    /* set condition shared between processes */
    pthread_condattr_t cattr3;
    pthread_condattr_setpshared(&cattr3, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(cond, &cattr3);
    pthread_condattr_destroy(&cattr3);
    /*************************************/
    for (i = 0; i < 3; i++)
    {
        switch (children_ids[i] = fork())
        {
        case -1:
            perror("Fork error");
            exit(-3);
        case 0:
            if (execl(children[i], "", NULL))
            {
                perror("Exec error");
                sleep(1);
                exit(1);
            }
        }
    }
    /**************************************/
    for (i = 0; i < 3; i++)
    {
        w = wait(&x);
        if (x != 0)
        {
            printf("Child process returned an error, killing all other children\n");
            for (i = 0; i < 3; i++)
            {
                if (children_ids[i] != w)
                {
                    sprintf(cmd, "kill -9 %d", children_ids[i]);
                    system(cmd);
                }
            }
        }
        else
        {
            fprintf(stdout, "Process %d returned code %d\n", w, x);
        }
    }


    shared_mutex_destroy(mutex);
    shared_mutex_destroy(mutex1);
    shared_mutex_destroy(mutex2);
    shared_mutex_destroy(mutex3);

    return 0;
}

void checkFile()
{
    FILE *fp = fopen("spr1.txt", "w");
    if (fp == NULL)
    {
        printf("couldn't open file. exiting...");
        exit(5);
    }
    fclose(fp);
}