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

shared_mutex_t mutex[4];
pthread_cond_t *cond[4];

int cond_id[4];

int i, w, x;
pid_t pid;
int children_ids[3];
char cmd[50], children[3][5] = {"p1", "p2", "p3"};
int mode = S_IRWXU | S_IRWXG;

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

int createMutex()
{
    for (i = 0; i < 4; i++)
    {
        char *mutex_name;
        sprintf(mutex_name, "/mutex%d", i);

        mutex[i] = shared_mutex_init("/testmutex0", mode);
        exit(0);
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
        cond_id[i] = shm_open(cond_name, O_CREAT | O_RDWR | O_TRUNC, mode);
        if (cond_id[i] < 0)
        {
            return -1;
        }
        if (ftruncate(cond_id[i], sizeof(pthread_cond_t)) == -1)
        {
            return -1;
        }
        cond[i] = (pthread_cond_t *)mmap(NULL, sizeof(pthread_cond_t), PROT_READ | PROT_WRITE, MAP_SHARED, cond_id[i], 0);
        if (cond[i] == MAP_FAILED)
        {
            return -1;
        }
        /* set condition shared between processes */
        pthread_condattr_t cattr;
        pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
        pthread_cond_init(cond[i], &cattr);
        pthread_condattr_destroy(&cattr);
    }
}

int main(int argc, char *argv[])
{
    checkFile();
    createMutex();

    if (createMutex())
    {
        printf("fail");
        return -1;
    }
    createCond();

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

    for (i = 0; i < 4; i++)
    {
        shared_mutex_destroy(mutex[i]);
    }

    return 0;
}
