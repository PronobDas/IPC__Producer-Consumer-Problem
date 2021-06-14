#include<iostream>
#include<cstdio>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include<cstring>
#include <time.h>

using namespace std;
#define number_of_producers 5
#define number_of_consumers 2
#define arr_size 5
pthread_mutex_t mutex;
sem_t full_array, empty_array;

int ind = 0;

void * produce_item(void *arg)
{
    //printf("I am producer %s. \n",(char *)arg);
    //pthread_exit((void *)strcat((char *)arg, "is finishing. \n"));
    while(true)
    //for(int i = 0; i<5; i++)
    {
        //printf(" Producer %s want to create product %d\n",(char *)arg, ind);
        sem_wait(&empty_array);
        pthread_mutex_lock(&mutex);

        sleep(rand()%4);
        printf("Producer %s has produced product %d.\n",(char *)arg, ind);
        ind++;
        pthread_mutex_unlock(&mutex);

        sem_post(&full_array);
    }
    pthread_exit((void*)strcat((char*)arg," producer is finishing\n"));
}
void * consume_item(void *arg)
{
    while(true)
    //for(int i = 0; i<5; i++)
    {
        //printf("  Consumer %s want to consume product %d\n",(char *)arg, ind);
        sem_wait(&full_array);
        pthread_mutex_lock(&mutex);

        sleep(rand()%2);
        printf("Consumer %s has consumed product %d.\n",(char *)arg, ind-1);
        ind--;
        pthread_mutex_unlock(&mutex);

        sem_post(&empty_array);
    }
    pthread_exit((void*)strcat((char*)arg," consumer is finishing\n"));
}

int main()
{
    int res;
    res = sem_init(&full_array,0,0);
    if(res != 0)
    {
        printf("Failed. \n");
    }

    res = sem_init(&empty_array,0,arr_size);
    if(res != 0)
    {
        printf("Failed. \n");
    }

    res = pthread_mutex_init(&mutex, NULL);
    if(res != 0)
    {
        printf("Failed. \n");
    }

    pthread_t producers[number_of_producers], consumers[number_of_consumers];

    for(int i = 0; i < number_of_producers; i++)
    {
        char *id = new char[3];
        strcpy(id,to_string(i+1).c_str());
        res = pthread_create(&producers[i],NULL,produce_item,id);
        if(res != 0)
        {
            printf("Producer%d's thread creation Failed.\n",i);
        }
    }

    for(int i = 0; i < number_of_consumers; i++)
    {
        char *id = new char[3];
        strcpy(id,to_string(i+1).c_str());
        res = pthread_create(&consumers[i],NULL,consume_item,id);
        if(res != 0)
        {
            printf("Consumer%d's thread creation Failed.\n",i);
        }
    }


    for(int i = 0; i < number_of_producers; i++)
    {
        void * result;
        pthread_join(producers[i],&result);
        printf("%s ",(char *)result);
    }
    for(int i = 0; i < number_of_consumers; i++)
    {
        void * result;
        pthread_join(consumers[i],&result);
        printf("%s ",(char *)result);
    }



    res = sem_destroy(&full_array);
    if(res != 0)
    {
        printf("Failed. \n");
    }
    res = sem_destroy(&empty_array);
    if(res != 0)
    {
        printf("Failed. \n");
    }
    res = pthread_mutex_destroy(&mutex);
    if(res != 0)
    {
        printf("Failed. \n");
    }
    return 0;
}
