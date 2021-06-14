#include<iostream>
#include<cstdio>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include<cstring>
#include <time.h>

using namespace std;
#define number_of_producers 2 //services
#define number_of_consumers 6 //cyclists

#define payment_booth_size 2
pthread_mutex_t mutex[number_of_producers];
pthread_mutex_t priority;

sem_t payment_serial;//full_array, empty_array;
//sem_t semarr[number_of_producers];

//int ind = 0;
int q = 0;
int count = 0;
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
    //pthread_mutex_lock(&priority);
    for(int i = 0; i<number_of_producers; i++)
    {
        if(i == 0){
            count++;
            pthread_mutex_lock(&priority);
        }

        pthread_mutex_lock(&mutex[i]);

        printf("%s started taking service from seviceman %d. \n",(char *)arg, i);

        if(i > 0 && i < number_of_producers)
        {
            pthread_mutex_unlock(&mutex[i-1]);
            //sem_post(&semarr[i-1]);
        }

        sleep(1); //rand()%2
        printf("%s has finished taking service from seviceman %d. \n",(char *)arg, i);
        //ind--;
        if(i == 0 && q == 0)
            pthread_mutex_unlock(&priority);
            sleep(1);
        if(i == number_of_producers-1)
        {
            pthread_mutex_unlock(&mutex[i]);
            //sem_post(&semarr[i]);
        }
        //sem_post(&empty_array);
    }
    //pthread_mutex_unlock(&priority);
    //cout << "count: "<< count << endl;


    printf("%s waiting for payment. \n",(char*)arg);
    count--;
    sem_wait(&payment_serial);
    printf("%s started paying service bill. \n",(char *)arg);
    sleep(1);
    printf("%s finished paying service bill. \n",(char *)arg);
    q++;
    sem_post(&payment_serial);


    //printf("Hello");
    pthread_mutex_lock(&priority);
    while(true)
    {
    //    printf("Hi");
        if(count == 0)
        {
            printf("%s has departed. \n",(char *)arg);
            q--;
            pthread_mutex_unlock(&priority);
            break;
        }
    }
    pthread_exit((void*)strcat((char*)arg," success\n"));
}

int main()
{
    int res;

/*    res = sem_init(&full_array,0,0);
    if(res != 0)
    {
        printf("Failed. \n");
    }

    res = sem_init(&empty_array,0,arr_size);
    if(res != 0)
    {
        printf("Failed. \n");
    }
*/
    res = sem_init(&payment_serial,0,payment_booth_size);
    if(res != 0)
    {
        printf("Failed. \n");
    }


    for(int i = 0; i < number_of_producers; i++)
    {
        res = pthread_mutex_init(&mutex[i], NULL); //sem_init(&semarr[i],0,1); //
        if(res != 0)
        {
            printf("Failed. \n");
        }
    }
    res = pthread_mutex_init(&priority, NULL);
    if(res != 0)
    {
        printf("Failed.\n");
    }


    pthread_t consumers[number_of_consumers]; //producers[number_of_producers]

/*    for(int i = 0; i < number_of_producers; i++)
    {
        char *id = new char[3];
        strcpy(id,to_string(i+1).c_str());
        res = pthread_create(&producers[i],NULL,produce_item,id);
        if(res != 0)
        {
            printf("Producer%d's thread creation Failed.\n",i);
        }
    }
*/

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

/*
    for(int i = 0; i < number_of_producers; i++)
    {
        void * result;
        pthread_join(producers[i],&result);
        printf("%s ",(char *)result);
    }
*/
    for(int i = 0; i < number_of_consumers; i++)
    {
        void * result;
        pthread_join(consumers[i],&result);
        //printf("Cyclist %s ",(char *)result);
    }

    /*res = sem_destroy(&full_array);
    if(res != 0)
    {
        printf("Failed. \n");
    }*/
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
    res = pthread_mutex_destroy(&priority);
    if(res != 0)
    {
        printf("Failed. \n");
    }
    return 0;
}
