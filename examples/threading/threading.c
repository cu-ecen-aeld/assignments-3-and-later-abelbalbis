#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>


// Optional: use these functions to add debug or error prints to your application
#define printfDEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{
    
    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    int rc;
    struct thread_data *t_data= (struct thread_data*)thread_param;
    
    
    usleep(t_data->wait_to_obtain_ms * 1000U);
   
    rc = pthread_mutex_lock(t_data->mtx);

    if ( rc != 0 ) {
        ERROR_LOG("pthread_mutex_lock failed with %d\n",rc);
    } else {
        t_data->thread_complete_success = true;
    }
    

    usleep(t_data->wait_to_release_ms * 1000U);
    pthread_mutex_unlock(t_data->mtx);
    
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    int s;
    struct thread_data *data = malloc(sizeof(struct thread_data));
    data->mtx = mutex;
    data->thread_complete_success = false;    
    data->wait_to_obtain_ms = wait_to_obtain_ms;
    data->wait_to_release_ms = wait_to_release_ms;


   s = pthread_create(thread, NULL, threadfunc, data);
    
    if (s != 0) {
        return false;
    }
    

    return true;
}

