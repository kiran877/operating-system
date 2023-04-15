#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define MAX_THREADS 10
#define MAX_RESOURCES 5

int available_resources = MAX_RESOURCES;
pthread_mutex_t mutex;

int decrease_count(int count) {
    pthread_mutex_lock(&mutex);
    if (available_resources < count) {
        pthread_mutex_unlock(&mutex);
        return -1;
    } else {
        available_resources -= count;
        printf("Allocated %d resources\n", count);
        printf("Available resources: %d\n", available_resources);
        pthread_mutex_unlock(&mutex);
        return 0;
    }
}

int increase_count(int count) {
    pthread_mutex_lock(&mutex);
    available_resources += count;
    printf("Released %d resources\n", count);
    printf("Available resources: %d\n", available_resources);
    pthread_mutex_unlock(&mutex);
    return 0;
}

void* thread_code(void* arg) {
    int* count = (int*) arg;
    int request = rand() % (*count) + 1;
    printf("Thread %lu requesting %d resources\n", (unsigned long)pthread_self(), request);
    if (decrease_count(request) == -1) {
        printf("Thread %lu waiting for resources\n", (unsigned long)pthread_self());
        while (decrease_count(request) == -1);
    }
    printf("Thread %lu got %d resources\n", (unsigned long)pthread_self(), request);
    sleep(2);
    increase_count(request);
    printf("Thread %lu released %d resources\n", (unsigned long)pthread_self(), request);
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Mutex initialization failed\n");
        return 1;
    }

    int num_threads;
    printf("Enter the number of threads: ");
    scanf("%d", &num_threads);

    if (num_threads > MAX_THREADS || num_threads < 1) {
        printf("Invalid number of threads\n");
        return 1;
    }

    pthread_t threads[num_threads];
    int thread_args[num_threads];

    srand(time(NULL));
    for (int i = 0; i < num_threads; i++) {
        thread_args[i] = MAX_RESOURCES / num_threads;
        pthread_create(&threads[i], NULL, thread_code, &thread_args[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}
