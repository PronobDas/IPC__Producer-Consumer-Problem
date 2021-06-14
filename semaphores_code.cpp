//#include<stdio.h>
//#include<pthread.h>
//#include<semaphore.h>
#include<queue>
//#include<unistd.h>
//#include<cstring>

#include<iostream>
#include<cstdio>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include<cstring>
#include <time.h>
using namespace std;


//semaphore to control sleep and wake up
sem_t empty;
sem_t full;
queue<int> q;
pthread_mutex_t lock;


void init_semaphore()
{
	sem_init(&empty,0,5);
	sem_init(&full,0,0);
	pthread_mutex_init(&lock,0);
}

void* ProducerFunc(void * arg)
{
	printf("I am %s\n",(char*)arg);
	int i;
	for(i=1;i<=10;i++)
	{
		sem_wait(&empty);

		pthread_mutex_lock(&lock);
		q.push(i);
		printf("%s produced item %d\n", (char*) arg, i);

		pthread_mutex_unlock(&lock);

		sem_post(&full);
	}
	pthread_exit(NULL);
}

void* ConsumerFunc(void * arg)
{
	printf("I am %s\n",(char*)arg);
	int i;
	for(i=1;i<=10;i++)
	{
		sem_wait(&full);

		pthread_mutex_lock(&lock);

		int item = q.front();
		q.pop();
		printf("%s consumed item %d\n", (char*) arg, item);

		pthread_mutex_unlock(&lock);

		sem_post(&empty);
	}
	pthread_exit(NULL);
}



int main(void)
{
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	pthread_t thread4;

	init_semaphore();

	char message1[] = "producer1";
	char message2[] = "consumer1";
	char message3[] = "producer2";
	char message4[] = "consumer2";

	//char *message = new char[3];
    //strcpy(message,to_string(1+1).c_str());

	pthread_create(&thread1,NULL,ProducerFunc,(char *)message1);
    pthread_create(&thread3,NULL,ProducerFunc,(char *)message3);

	pthread_create(&thread2,NULL,ConsumerFunc,(char *)message2);
	pthread_create(&thread4,NULL,ConsumerFunc,(char *)message4);


	while(1);
	return 0;
}
