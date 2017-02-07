/*
Semaphore example
Build using
gcc -o eighthproc eighth.o -lpthread -lrt
*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void *doSomething1();
void *doSomething2();
void *doSomething3();

sem_t sem;   //declare semaphore (should be global)

int main() {
    // initialize sem to 2 share with only current process
    sem_init(&sem, 0, 2);

    pthread_t thread1, thread2, thread3; //handle to threads

    //Create 3 threads pointing to doSomething1,2,3 respectively
    pthread_create(&thread1, NULL, &doSomething1, NULL);
    pthread_create(&thread2, NULL, &doSomething2, NULL);
    pthread_create(&thread3, NULL, &doSomething3, NULL);

    //wait for all threads to finish and rejoin before exiting process
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    return 0;
}

void doSomething(char c) {
    int i, time;
    for (i = 0; i < 3; i++) {

        // DOWN or P operation
        if (sem_wait(&sem) == 0) {

            // generate random amount of time (< 5 seconds)
            time = (int) ((double) rand() / RAND_MAX * 5);

            printf("Thread %c enters and sleeps for %d seconds...\n", c, time);

            sleep(time);

            printf("Thread %c leaves the critical section\n", c);

            //UP or V operation
            sem_post(&sem);
        }
    }
}

void *doSomething1() {
    // thread A
    doSomething('A');

    return 0;
}

void *doSomething2() {
    // thread B
    doSomething('B');

    return 0;
}
void *doSomething3() {
    // thread B
    doSomething('C');

    return 0;
}
