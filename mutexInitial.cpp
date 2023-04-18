#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdint>
#include <iostream>
using namespace std;
#define COUNT_TRIGGER 10
#define COUNT_LIMIT 12

int count = 0; 
int thread_ids[3] = {0,1,2};
// thread will count to 20 because both threads are inside right behind each other
pthread_mutex_t count_mutex; // var to increment
pthread_cond_t count_cv; // condition var

// adds to global counter var
// by exclusive active
void* add_count(void* t)
{
    int tid = (intptr_t) t; 
    cout << endl << "A Thread has entered add_count function" << endl; 
    for (int i = 0; i < COUNT_TRIGGER; ++i)
    {
        cout << endl << "Mutex is going to be locked" << endl;
        pthread_mutex_lock(&count_mutex); // obtained exclusive access 
        count++;
        if (count == COUNT_LIMIT) // checks for limit
        {
            pthread_cond_signal(&count_cv); // signals condition var
        }
        cout << endl << "Mutex is going to be unlocked. " << endl;
        pthread_mutex_unlock(&count_mutex); // gives up access
        cout << endl << "The count is at: " << count << endl;
        sleep(1); // sleep to give a chance for other variable to enter
    }
    pthread_exit(0);
}

void* watch_count(void* t)
{
    int tid = (intptr_t) t;
    cout << endl << "Thread 1 is watching count." << endl;
    pthread_mutex_lock(&count_mutex); // lock the count var, insurance for exclusion
    if (count < COUNT_LIMIT) // check for limit
    {
        // wait on condition variable, providing count var and locked mutex
        cout << endl << "Thread 1 is now waiting." << endl;
        pthread_cond_wait(&count_cv, &count_mutex);
        cout << endl << "Count limit at: " << count_mutex << endl;
    }
    // once signaled, unlock and exit
    pthread_mutex_unlock(&count_mutex);
    pthread_exit(0);
}

int main (int argc, char* argv[])
{
    int tid1 = 1, tid2 = 2, tid3 = 3;
    pthread_t threads[3];
    pthread_attr_t attr;
    cout << "Thread Initialized." << endl; 
    pthread_mutex_init(&count_mutex, 0);
    pthread_cond_init(&count_cv, 0);
    cout << "Mutex and Condition Variable Initialized." << endl; 
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    // 2 threads to count, 1 to wait for condition variable
    pthread_create(&threads[0], &attr, watch_count, (void*) tid1);
    cout << "Thread 1 set watching count." << endl;
    pthread_create(&threads[1], &attr, add_count, (void*) tid2);
    cout << "Thread 2 set adding count" << endl;
    pthread_create(&threads[2], &attr, add_count, (void*) tid3);
    cout << "Thread 3 set adding count" << endl;

    
    for (int i = 0; i < 3; ++i)
        pthread_join(threads[i], 0);
    cout << "Threads joined." << endl;
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&count_mutex);
    pthread_cond_destroy(&count_cv); 
    return 0; 

}