#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFF 5 //max buffer size

void *Producer();
void *Consumer();
void insert_widget(int);
int produce_widet();
void remove_widget();
void consume_widget();


sem_t crit_reg;	//sem for making sur only one thread enter critical region
sem_t full;	    //counts number of full "buffer slots"
sem_t empty;	//counts number of empty "buffer slots"

int shared_data; //shared buffer, starting with zero widget

int main()
{
	sem_init(&crit_reg, 0, 1);
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, BUFF);

	pthread_t prod, cons;
	pthread_create(&prod, NULL, &Producer, NULL);
	pthread_create(&cons, NULL, &Consumer, NULL);
	pthread_join(prod, NULL);
	pthread_join(cons, NULL);

	return 0;
}


void *Producer(){
	int widget;

	while(1){
        //if(shared_data == 5){pthread_yield();} //If the widget buffer is maxed, release cpu
		widget = produce_widget();
		sem_wait(&empty);
		sem_wait(&crit_reg);
		//printf("in producer critical region\n");
		insert_widget(widget);
		printf("shared: %d \n", shared_data);
		sem_post(&crit_reg);
		sem_post(&full);
	}
}

void *Consumer()
{

	while(1){
		sem_wait(&full);
		sem_wait(&crit_reg);
		//printf("in consumer critical region\n");
		remove_widget();
		printf("shared: %d \n", shared_data);
		sem_post(&crit_reg);
		sem_post(&empty);
		consume_widget();
		//if(!shared_data){pthread_yield();} //no more widgets, rather than looping, release CPU
	}
}

int produce_widget()
{
    //printf("producing widget\n");
	return 1;
}
void insert_widget(int item)
{
	shared_data += item;
	return;
}
void remove_widget()
{
	shared_data--;
	return;
}
void consume_widget()
{
	//printf("widget consumed\n");
	return;
}
