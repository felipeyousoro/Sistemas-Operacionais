#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t tic_sem, tac_sem, toe_sem;

void* tic_fun(void* arg) {
    while(1){
        sem_wait(&toe_sem);
        printf("TIC ");
        fflush(stdout);
        sleep(0.5);
        sem_post(&tic_sem);
    }
}

void* tac_fun(void* arg) {
    while(1){
        sem_wait(&tic_sem);
        printf("TAC ");
        fflush(stdout);
        sleep(0.5);
        sem_post(&tac_sem);
    }    
}

void* toe_fun(void* arg) {
    while(1){
        sem_wait(&tac_sem);
        printf("TOE \n");
        fflush(stdout);
        sleep(1.5);
        sem_post(&toe_sem);
    }    
}

int main() { 
    pthread_t tic_thrd, tac_thrd, toe_thrd;
    
    sem_init(&tic_sem, 0, 0);
    sem_init(&tac_sem, 0, 0);
    sem_init(&toe_sem, 0, 1);
    
    pthread_create(&tic_thrd, NULL, tic_fun, NULL);
    pthread_create(&tac_thrd, NULL, tac_fun, NULL);
    pthread_create(&toe_thrd, NULL, toe_fun, NULL);
    
    pthread_join(tic_thrd, NULL);
    pthread_join(tac_thrd, NULL);
    pthread_join(toe_thrd, NULL);

    return 0;
}