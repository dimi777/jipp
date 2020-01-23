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
#include <pthread.h>
#include <stdio_ext.h>
#include <sys/errno.h>

#define MAX 255
#define SERWER 1

int semafor;
int stworz_watek[2];
int przylacz_watek[2];
int odlacz_watek[2];
int wysylam = 1;
int i;

struct arg
{
    long nadawca;
    int IDkolejki;
};

//struktura komunikatu
struct komunikat
{
    long mtype;
    long nadawca;
    char mtext[MAX];
};

void *send(void *args)
{
    int IDkolejki;
    char c;
    int j = 0;
    struct arg *data;
    struct komunikat kom;
    data = (struct arg *)args;
    IDkolejki = data->IDkolejki;
    do
    {
        kom.mtype = SERWER;
        printf("Podaj tresc komunikatu: ");
        __fpurge(stdin);
        for (j = 0; j < MAX; j++)
            kom.mtext[j] = '\0';
        fgets(kom.mtext, MAX, stdin);
        j = 0;
        while (kom.mtext[j] != '\n')
            j++;
        kom.mtext[j] = '\0';
        kom.mtext[MAX - 1] = '\0';
        kom.nadawca = data->nadawca;
        if (msgsnd(IDkolejki, &kom, strlen(kom.mtext) + sizeof(long) + 1, IPC_NOWAIT) == -1)
        {
            printf("Nie udalo sie wyslac.\nKoncze dzialanie watku.\n");
            wysylam = 0;
            pthread_exit((void *)3);
        }
        i++;
        printf("Chcesz wyslac kolejna wiadomosc?(Jesli tak wpisz \'t\' jesli nie wpisz cokolwiek innego)\n");
        __fpurge(stdin);
        c = fgetc(stdin);
    } while (c == 't');
    wysylam = 0;
    //if(msgsnd(IDkolejki,&kom,strlen(kom.mtext)+sizeof(long)+1,IPC_NOWAIT)==-1){
    //	pthread_exit((void*)3);
    //}
    //i++;
    pthread_exit(0);
}

void *receive(void *args)
{
    int IDkolejki;
    int flg = MSG_NOERROR;
    struct komunikat kom;
    struct arg *data;
    data = (struct arg *)args;
    IDkolejki = data->IDkolejki;
    while (wysylam == 1 || i > 0)
    {
        kom.mtype = data->nadawca;
        while (wysylam == 1 && i == 0)
            ;
        if (wysylam == 0)
        {
            flg = MSG_NOERROR | IPC_NOWAIT;
        }
        else
            flg = MSG_NOERROR;
        if (msgrcv(IDkolejki, &kom, MAX + sizeof(long), kom.mtype, flg) == -1)
        {
            //printf("TEST\n");
            if (errno == ENOMSG)
                pthread_exit(0);
            printf("Nie udalo sie pobrac komunikatu.\n");
        }
        i--;
        printf("Klient %ld: Odebrano: \'%s\' zaadresowane do %ld\n", data->nadawca, kom.mtext, kom.mtype);
        if (wysylam == 0)
        {
            flg = MSG_NOERROR | IPC_NOWAIT;
            sleep(3);
        }
    }
    pthread_exit(0);
}

int mCreate(key_t key);
void create_sem(key_t key);
void zamknij_serwer();

int main()
{
    pthread_t id_watku;
    pthread_t id_watku2;
    long klient;
    char c;
    int val = 0;
    void *ret = NULL;
    int IDkolejki;
    struct komunikat kom;
    struct arg argumenty;
    int semctl_check;
    create_sem(69);
    if (semctl(semafor, 0, GETVAL, val) == 0)
    {
        printf("Server wylaczony.\n");
        exit(EXIT_SUCCESS);
    }
    IDkolejki = mCreate(50);
    printf("id=%d\n", IDkolejki);
    klient = getpid();
    argumenty.IDkolejki = IDkolejki;
    argumenty.nadawca = klient;
    printf("Chcesz wylaczyc serwer czy wysylac wiadomosci? (Wpisz \'w\' jesli chcesz wylaczyc serwer jesli chcesz wysylac wpisz inny znak)\n");
    __fpurge(stdin);
    c = fgetc(stdin);
    if (c != 'w')
    {
        stworz_watek[0] = pthread_create(&id_watku, NULL, send, (void *)&argumenty);
        stworz_watek[1] = pthread_create(&id_watku2, NULL, receive, (void *)&argumenty);
        if (stworz_watek[0] == -1 || stworz_watek[1] == -1)
        {
            printf("Tworzenie watkow fail.\n");
            exit(EXIT_FAILURE);
        }
        przylacz_watek[1] = pthread_join(id_watku, NULL);
        przylacz_watek[0] = pthread_join(id_watku2, &ret);
        if (przylacz_watek[0] == -1 || przylacz_watek[1] == -1)
        {
            printf("Przylaczanie fail.\n");
            exit(EXIT_FAILURE);
        }
        printf("Watek odbierajacy zakonczony. (%d)\n", ret);
        odlacz_watek[0] = pthread_detach(id_watku);
        odlacz_watek[1] = pthread_detach(id_watku2);
        if (odlacz_watek[0] == -1 || odlacz_watek[1] == -1)
        {
            printf("Odlaczaniei fail.\n");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    else
    {
        zamknij_serwer(IDkolejki, klient);
        exit(EXIT_SUCCESS);
    }
}

int mCreate(key_t key)
{
    int IDkolejki;
    if ((IDkolejki = msgget(key, IPC_CREAT | 0600)) == -1)
    {
        printf("Blad tworzenia kolejki.\n");
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
}

void zamknij_serwer(int IDkolejki, long klient)
{
    struct komunikat kom;
    int i;
    if (semctl(semafor, 0, SETVAL, 0) == -1)
    {
        printf("Nie udalo sie ustawic semafora.\n");
        exit(EXIT_FAILURE);
    }
    kom.mtype = SERWER;
    for (i = 0; i < MAX; i++)
        kom.mtext[i] = '\0';
    kom.mtext[0] = 'k';
    kom.mtext[1] = 'o';
    kom.mtext[2] = 'n';
    kom.mtext[3] = 'i';
    kom.mtext[4] = 'e';
    kom.mtext[5] = 'c';
    printf("%s\n", kom.mtext);
    kom.nadawca = klient;
    if (msgsnd(IDkolejki, &kom, strlen(kom.mtext) + sizeof(long) + 1, IPC_NOWAIT) == -1)
    {
        printf("NIe udalo sie");
        exit(EXIT_FAILURE);
    }
    if (msgrcv(IDkolejki, &kom, MAX + sizeof(long), klient, MSG_NOERROR) == -1)
    {
        printf("NIe udalo sie");
        exit(EXIT_FAILURE);
    }
}