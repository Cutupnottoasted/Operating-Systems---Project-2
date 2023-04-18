#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdint>
#include <iostream>
using namespace std;
pthread_key_t global_var_key;

void* worker (void* arg)
{
    cout << endl << "Thread in worker function" << endl;
    int *p = new int;
    *p = 1;

    pthread_setspecific(global_var_key, p);

    int* global_spec_var = (int*) pthread_getspecific (global_var_key);
    *global_spec_var += 1;
    cout << endl << "global val incremented " << endl;
    pthread_setspecific(global_var_key, 0);
    delete p;
    cout << endl << "P deleted" << endl;
    pthread_exit(0);
}

int main (void) 
{
    pthread_t threads[5];
    pthread_key_create(&global_var_key, 0);
    cout << endl << "pthread key created" << endl;
    for (int i = 0; i < 5; ++i)
    {
        cout << endl << "Thread " << i << " created" << endl;
        pthread_create(&threads[i], 0, worker, 0);
        cout << endl << "Thread " << i << " entering worker function" << endl;
    }
    
    for (int i = 0; i < 5; ++i)
        pthread_join(threads[i], 0);
    
    return 0;
}