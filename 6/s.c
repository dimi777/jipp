#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>

#define MAX 255
#define SERWER 1

int semafor;

//struktura komunikatu
struct komunikat
{
    long mtype;
    long nadawca;
    char mtext[MAX];
};

int mCreate(key_t key);
void mDelete(int IDkolejki);
void create_sem(key_t key);
void delete_sem(void);
void koniec(int);

int main()
{
    int msize;
    int flg = MSG_NOERROR;
    int i, pid;
    int IDkolejki;
    struct komunikat kom;
    struct msqid_ds qstat;
    int run = 1;
    int pom = 1;
    int semctl_check = 1;
    create_sem(69);
    IDkolejki = mCreate(50);
    printf("id=%d\n", IDkolejki);
    while (run)
    {
        kom.mtype = SERWER;
        if (semctl_check != 0)
            printf("Serwer: Czekam na komunikat ...\n");
        if (msgrcv(IDkolejki, &kom, MAX + sizeof(long), SERWER, flg) == -1)
        {
            if (errno == ENOMSG && qstat.msg_qnum != 0)
                pom = 0;
            if (errno == ENOMSG && qstat.msg_qnum == 0)
                koniec(IDkolejki);
        }
        else
            pom = 1;
        if (pom)
        {
            printf("Serwer: Odebrano: \"%s\" zaadresowane do %ld\n", kom.mtext, kom.mtype);

            msize = strlen(kom.mtext);
            for (i = 0; i < msize; i++)
            {
                kom.mtext[i] = toupper(kom.mtext[i]);
            }
            //wysylanie

            kom.mtype = kom.nadawca;
            kom.nadawca = SERWER;

            printf("Serwer: Wysylanie... %s -> %ld \n", kom.mtext, kom.mtype);
            //dodanie komunikatu do kolejki

            if (msgsnd(IDkolejki, &kom, strlen(kom.mtext) + sizeof(long) + 1, IPC_NOWAIT) == -1)
            {
                printf("Blad dodania komunikatu do kolejki.\n");
                exit(EXIT_FAILURE);
            }
        }
        if (msgctl(IDkolejki, IPC_STAT, &qstat) == -1)
        {
            printf("Nie udalo sie pobrac informacji o kolejnce.\n");
            exit(EXIT_FAILURE);
        }
        if ((semctl_check = semctl(semafor, 0, GETVAL, 0)) == -1)
        {
            printf("Blad pobrania warosci semafora.\n");
            exit(EXIT_FAILURE);
        }
        if (semctl_check == 0)
        {
            flg = MSG_NOERROR | IPC_NOWAIT;
        }
    }
    koniec(IDkolejki);
}

void koniec(int IDkolejki)
{
    sleep(1);
    mDelete(IDkolejki);
    delete_sem();
    exit(EXIT_SUCCESS);
}

void mDelete(int IDkolejki)
{
    if (msgctl(IDkolejki, IPC_RMID, 0) == -1)
    {
        printf("Blad usuniecia kolejki.\n");
        exit(EXIT_FAILURE);
    }
}

int mCreate(key_t key)
{
    int IDkolejki;
    if ((IDkolejki = msgget(key, IPC_CREAT | 0600)) == -1)
    {
        printf("Blad tworzenia kolejki.\n");
        delete_sem();
        exit(EXIT_FAILURE);
    }
    return IDkolejki;
}

void create_sem(key_t key)
{
    semafor = semget(key, 1, IPC_CREAT | 0600);
    if (semafor == -1)
    {
        printf("Nie udalo sie stworzyc semafora.\n");
        exit(EXIT_FAILURE);
    }
    if (semctl(semafor, 0, SETVAL, 1) == -1)
    {
        printf("Nie udalo sie ustawic semafora.\n");
        exit(EXIT_FAILURE);
    }
}

void delete_sem(void)
{
    int del_check;
    del_check = semctl(semafor, 0, IPC_RMID);
    if (del_check == -1)
    {
        printf("Nie udalo sie usunac semafora.\n");
        exit(EXIT_FAILURE);
    }
}
