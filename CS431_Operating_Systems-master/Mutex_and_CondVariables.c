/*
 Example showing MUTEX and CONDITION VARIABLES in C
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//Statically creating and initalizing a MUTEX -->just a lock [GLOBAL]
pthread_mutex_t sum_mutex = PTHREAD_MUTEX_INITIALIZER;

//Create a MUTEX -->just a lock [GLOBAL]
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;

//create a CONDITION VARIABLE [GLOBAL]
pthread_cond_t condition_cond = PTHREAD_COND_INITIALIZER;

void *functionAdd1();
void *functionAdd2();

//Global Variables and Constants
int sum = 0;
#define DONE 10
#define STOP1 2
#define STOP2 4
int count=0;

int main()
{
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, &functionAdd1, NULL);
    pthread_create(&thread2, NULL, &functionAdd2, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    exit(0);
}

void *functionAdd1()
{
    while(1)
    {
      // ATTEMPT TO LOCK will either go to WHILE loop or SLEEP
        pthread_mutex_lock(&condition_mutex);
            while (sum >= STOP1 && sum <=STOP2){
	      //NEED TO WAIT FOR SOMETHING TO HAPPEN

	      //Block Thread1 until condition is met(wait for signal)
	      //and release MUTEX lock
                pthread_cond_wait(&condition_cond, &condition_mutex);
            }
        pthread_mutex_unlock(&condition_mutex);

        pthread_mutex_lock(&sum_mutex);
            sum++;
            printf("Sum value functionAdd1: %d\n",sum);
        pthread_mutex_unlock(&sum_mutex);

        if (sum >= DONE) return (NULL);
    }
}

void *functionAdd2()
{
    while(1)
    {
        pthread_mutex_lock(&condition_mutex);
            if(sum < STOP1 || sum > STOP2)
            {
	      // SIGNAL ONLY ONE THREAD vs BROADCAST
                pthread_cond_signal(&condition_cond);
            }
        pthread_mutex_unlock(&condition_mutex);

        pthread_mutex_lock(&sum_mutex);
            sum++;
            printf("Sum value functionAdd2: %d\n",sum);
        pthread_mutex_unlock(&sum_mutex);

        if (sum >=DONE) return (NULL);
    }
}

