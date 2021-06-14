#include<iostream>
#include<cstdio>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include<cstring>
#include <time.h>

#define cycles 10
#define no_of_serviceman 3
#define cap_booth 2

using namespace std;

pthread_mutex_t serviceman[no_of_serviceman];
sem_t booth;
int no_of_cyclists_waiting_for_departure = 0;

void* takeService(void* arg){
    // In this loop, each cyclist is taking service sequentially from the servicemen
    for(int i=0;i<no_of_serviceman;i++){
        pthread_mutex_lock(&serviceman[i]);
        if((!i) && no_of_cyclists_waiting_for_departure){// If someone is waiting for departure, I am leaving space for it
            pthread_mutex_unlock(&serviceman[i]);
            i--;
            continue;
        }
        printf("%s started taking service from serviceman %d\n",(char*)arg,i+1);
        if(i!=0)pthread_mutex_unlock(&serviceman[i-1]); // the previous serviceman's lock must be unlocked after locking the current serviceman's
        //lock, otherwise it may be the case that one cyclist unlocked the serviceman but before going to the next serviceman, next cyclist started
        //taking this serviceman's service
        int waitTime = rand()%3 + 2; //generate random wait time between 2 to 4 seconds
        sleep(waitTime);
        printf("%s finished taking service from serviceman %d\n",(char*)arg,i+1);
    }
    pthread_mutex_unlock(&serviceman[no_of_serviceman-1]); // unlocking the last serviceman

    // After finishing service, this cyclist is going to pay
    sem_wait(&booth);
    printf("%s started paying the service bill\n",(char*)arg);
    int paymentTime = rand()%3 + 2;
    sleep(paymentTime);
    printf("%s finished paying the service bill\n",(char*)arg);
    sem_post(&booth);
	
	// nicher line er charpashe ekta lock dite hobe, jeta ami dute vule gesilam
    no_of_cyclists_waiting_for_departure++;

    // Now the cyclist will depart, for departure it needs to stop each of the serviceman from giving service
    for(int i=0;i<no_of_serviceman;i++){
        pthread_mutex_lock(&serviceman[i]);
    }

    sleep(1); //1s time for it to depart
    printf("%s has departed\n",(char*)arg);
    no_of_cyclists_waiting_for_departure--;
    //After departure, all the serviceman are now free
    for(int i=no_of_serviceman-1;i>=0;i--){
        pthread_mutex_unlock(&serviceman[i]);
    }
}

int main(int argc,char* argv[])
{
    int res;
    srand(time(NULL));
    // creating locks for every serviceman
    for(int i=0;i<no_of_serviceman;i++){
        int res = pthread_mutex_init(&serviceman[i],NULL);
        if(res != 0){
            cout << "Lock initialization failed\n";
            exit(1);
        }
    }

    //creating a semaphore for the booth
    res = sem_init(&booth,0,cap_booth);
    if(res != 0){
        cout << "Semaphore cretaion failed\n";
        exit(1);
    }

    //int min_priority = sched_get_priority_min(SCHED_FIFO);
    //struct sched_param scheduling_value;

    // creating a thread for each cyclist
    pthread_t cyclists[cycles];
    for(int i=0;i<cycles;i++){
        int res;
        char *id = new char[3];
        strcpy(id,to_string(i+1).c_str());

        /*pthread_attr_t thread_attr;
        res = pthread_attr_init(&thread_attr);
        if(res != 0){
            cout << "Attribute Creation failed\n";
            exit(1);
        }
        res = pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);
        if (res != 0) {
            printf("Setting scheduling policy failed");
            exit(1);
        }

        scheduling_value.sched_priority = min_priority + i + 1;
        res = pthread_attr_setschedparam(&thread_attr, &scheduling_value);
        if (res != 0) {
            printf("Setting scheduling priority failed");
            exit(1);
        }*/

        res = pthread_create(&cyclists[i],NULL,takeService,(void*)(id));
        if(res != 0){
            cout << "Thread Creation failed\n";
            exit(1);
        }
    }

    //Joining the threads with main thread
    for(int i=0;i<cycles;i++){
        void *result;
        int res = pthread_join(cyclists[i],&result);
        if(res != 0){
            cout << "Thread Join failed\n";
            exit(1);
        }
    }

    // destroying locks for every servicemane
    for(int i=0;i<no_of_serviceman;i++){
        int res = pthread_mutex_destroy(&serviceman[i]);
        if(res != 0){
            cout << "Lock destruction failed\n";
            exit(1);
        }
    }


    //destroying the booth semaphore
    res = sem_destroy(&booth);
    if(res != 0){
        cout << "Semaphore destruction failed\n";
        exit(1);
    }
    return 0;
}
