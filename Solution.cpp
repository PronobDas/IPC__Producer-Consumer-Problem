#include<iostream>
#include<cstdio>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include<cstring>
#include <time.h>

using namespace std;
#define number_of_producers 3 //services
#define number_of_consumers 5 //cyclists

#define payment_booth_size 2
pthread_mutex_t mutex[number_of_producers];
//pthread_mutex_t priority;
sem_t payment_serial;     //full_array, empty_array;

//int count = 0;
//int q = 0;
//int ind = 0;
/*void * produce_item(void *arg) //service_unit
{
    //printf("I am producer %s. \n",(char *)arg);
    //pthread_exit((void *)strcat((char *)arg, "is finishing. \n"));
    while(true)
    {
        //printf(" Producer %s want to create product %d\n",(char *)arg, ind);
        sem_wait(&empty_array);
        pthread_mutex_lock(&mutex);

        sleep(1); //rand()%4
        printf("Producer %s has produced product %d.\n",(char *)arg, ind);
        ind++;
        pthread_mutex_unlock(&mutex);

        sem_post(&full_array);
    }
    pthread_exit((void*)strcat((char*)arg," producer is finishing\n"));
}*/

void * consume_item(void *arg) //taking_service
{
    //while(true)
    for(int i = 0; i<number_of_producers; i++)
    {
        pthread_mutex_lock(&mutex[i]);

        /*if(i==0)
        {
            count++;
            pthread_mutex_lock(&priority);
        }
        else if(i == 1 && q == 0)
        {
            pthread_mutex_unlock(&priority);
        }*/

        printf("%s started taking service from serviceman %d\n",(char *)arg, i+1);

        if(i > 0 && i < number_of_producers)
        {
            pthread_mutex_unlock(&mutex[i-1]);
        }

        sleep(rand()%4);
        printf("%s finished taking service from serviceman %d\n",(char *)arg, i+1);
        printf("%s waiting for payment serial\n",(char*)arg);

        if(i == number_of_producers-1)
        {
            pthread_mutex_unlock(&mutex[i]);
        }
    }
    //count--;

    sem_wait(&payment_serial);
    printf("%s started paying the service bill\n",(char *)arg);
    sleep(rand()%3);
    printf("%s finished paying the service bill\n",(char *)arg);
    sem_post(&payment_serial);
    //q++;

    /*pthread_mutex_lock(&priority);
    while(true)
    {
        if(count == 0)
        {
            printf("%s has departed. \n",(char *)arg);
            q--;
            sleep(rand()%3);
            pthread_mutex_unlock(&priority);
            break;
        }
    }*/
    pthread_exit((void*)strcat((char*)arg," SUCCESS\n"));
}

int main()
{
    int res;

    res = sem_init(&payment_serial,0,payment_booth_size);
    if(res != 0)
    {
        printf("Failed. \n");
    }
    for(int i = 0; i < number_of_producers; i++)
    {
        res = pthread_mutex_init(&mutex[i], NULL);
        if(res != 0)
        {
            printf("Failed. \n");
        }
    }
    /*res = pthread_mutex_init(&priority, NULL);
    if(res != 0)
    {
        printf("Failed. \n");
    }*/

    pthread_t consumers[number_of_consumers]; //producers[number_of_producers]

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

    for(int i = 0; i < number_of_consumers; i++)
    {
        void * result;
        pthread_join(consumers[i],&result);
       // printf("%s",(char *)result);
    }


    res = sem_destroy(&payment_serial);
    if(res != 0)
    {
        printf("Failed. \n");
    }
    for(int i =0; i < number_of_producers; i++)
    {
        res = pthread_mutex_destroy(&mutex[i]);
        if(res != 0)
        {
            printf("Failed. \n");
        }
    }
    /*res = pthread_mutex_destroy(&priority);
    if(res != 0)
    {
        printf("Failed. \n");
    }*/
    return 0;
}
