#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>


/*************************/
/*  Recursos do programa */
#define RAND_UPPER_BOUND 31415

void *shared_queue;
sem_t sem_prod, sem_cons, mutex;
pthread_t thrd_prod, thrd_cons;

/************************/

typedef struct queue_node_t {
    int value;
    struct queue_node_t *next;

} queue_node_t;

typedef struct queue_t {
    queue_node_t *head, *tail;

} queue_t;

void enqueue(queue_t* queue, int value) {
    queue_node_t* node = (queue_node_t*) malloc(sizeof(queue_node_t));
    node->value = value;
    node->next = NULL;
    if(queue->head == NULL){
        queue->head = node;
        queue->tail = node;
    }
    else{
        queue->tail->next = node;
        queue->tail = node;
    }
}

int dequeue(queue_t* queue) {
    if(queue->head == NULL){
        return -1;
    }else{
        queue_node_t* node = queue->head;
        int value = node->value;
        queue->head = (queue_node_t*) node->next;
        free(node);
        return value;
    }
}

int top(queue_t* queue) {
    if(queue->head == NULL){
        return -1;
    }else{
        return queue->head->value;
    }
}

void* producer(void* arg) {
    while(1){
        sem_wait(&sem_prod);
        sem_wait(&mutex);

        int i = rand() % RAND_UPPER_BOUND;
        enqueue((queue_t*) shared_queue, i);

        printf("Produced: %d\n", i);
        fflush(stdout);

        sem_post(&mutex);
        sem_post(&sem_cons);        
    }
}

void* consumer(void* arg) {
    while(1){
        sem_wait(&sem_cons);
        sem_post(&mutex);
        
        int i = dequeue((queue_t*) shared_queue);

        printf("Consumed: %d\n", i);

        i = top((queue_t*) shared_queue);
        if(i == -1){
            printf("Queue is now empty\n\n");
            fflush(stdout);
        }
        else{
            printf("Next value in the queue: %d\n\n", i);
        }

        fflush(stdout);
        sleep(1);

        sem_post(&mutex);
        sem_post(&sem_prod);
    }
}

int main() {
    shared_queue = (queue_t*) malloc(sizeof(queue_t));

    sem_init(&sem_prod, 0, 1);
    sem_init(&sem_cons, 0, 0);
    sem_init(&mutex, 0, 1);

    pthread_create(&thrd_prod, NULL, producer, NULL);
    pthread_create(&thrd_cons, NULL, consumer, NULL);

    pthread_join(thrd_prod, NULL);
    pthread_join(thrd_cons, NULL);

    return 0;
}
