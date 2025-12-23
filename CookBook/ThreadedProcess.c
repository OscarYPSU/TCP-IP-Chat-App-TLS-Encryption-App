//
// Created by oskhy on 12/22/2025.
//
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void* count(void* arg){
    int id = *(int*)arg;
    for (int i = 1; i <= 10; i ++){
        printf("Thread %d: %d\n", id, i);
        sleep(1);
    }
    return NULL;
}

int main(){
    pthread_t t1, t2;
    int id1 = 1 , id2 = 1;

    pthread_create(&t1, NULL, count, &id1);
    pthread_create(&t2, NULL, count, &id2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}