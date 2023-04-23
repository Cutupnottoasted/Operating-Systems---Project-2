#include <pthread.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
#define NUM_THREADS 6

//static pthread_mutex_t mutexes[NUM_THREADS]; // initialize in main
static pthread_mutex_t func_mutex = PTHREAD_MUTEX_INITIALIZER; 

void func()
{
    pthread_mutex_lock(&func_mutex);

    pthread_mutex_unlock(&func_mutex);
}

void* worker(void* arg)
{
    cout << endl << "arg " << &arg << " inside worker function" << endl;
    int value = *((int*) arg);
    return 0;
}



int main(int argc, char** argv)
{
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    int result_code;

    // pthread initlization
    for (unsigned int i = 0; i < NUM_THREADS; ++i)
    {
        cout << endl << "pthread intialzing at i: " << i << endl; 
        thread_args[i] = i;
        // gets thread ID from pthread_create. &threads[i] is call method
        // 0 is for configuration. worker is a pointer to the function.
        // void* &thread_args[i] is passing the ID 
        result_code = pthread_create(&threads[i],0,worker,(void*) &thread_args[i]);
        cout << endl << "created thread[i]: (out of order = prev thread in worker function) " << threads[i] << endl << "at i " << i << endl << "thread_args[i]: (out of order = prev thread in worker function) " << thread_args[i] <<endl;
    }

    // pthread_join function waits for each thread to finish it's process  
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        cout << endl << "thread " << threads[i] << " joined" << endl;
        result_code = pthread_join(threads[i], 0); // takes ID, and buffer for return value of worker function
        cout << endl << "return value for join loop i " << i << ": " << result_code <<endl; 
    }

    exit(0);
}