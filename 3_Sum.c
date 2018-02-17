/* This program calculates the summation of numbers from 0 to a non-negative interger. All the numbers are divided into three groups with approximately equal size. Three child threads are created and the summation of all the numbers in one group calculated in a separate child thread. The parent thread adds the summation results obtained from all child threads and prints out the overall summation of all the numbers. */

//IMPORTANT: This is synchronous threading identified by the parent waiting for the children to terminate before it finishes its own execution. In Synchronous threading, the goal is make the children do a shared calculation and then have the parent gather the results once they are all done. All the children are created at the same time and run in sync.
//Opposite this to asynchronous threading, where the parent would run alongside the children and none of the children and parents are in sync with each other.

#include <pthread.h> /* header file for Pthread API */
#include <stdio.h>

#define THREAD_NUM 3 /* number of child threads */

int sum_thread[THREAD_NUM]; /* summation results obtained from child threads */
int sum = 0; /* overall summation of all numbers */
void *runner(void *param); /* child threads call this function */

struct Section {
    int index;
    int start;
    int end;
}; /* structure with index of child thread, the starting number and the ending number of the group of numbers allocated to a child thread */

int main(int argc, char *argv[])
{    
    int i;

    struct Section section[THREAD_NUM]; /* divide all numbers into three groups and allocate one group of numbers to a separate child thread */
    for (i=0; i<THREAD_NUM; i++)
    {
        if (i<THREAD_NUM-1)
        {
            section[i].index = i;
            section[i].start = atoi(argv[1])/THREAD_NUM*i;
            section[i].end = atoi(argv[1])/THREAD_NUM*(i+1)-1;
        }
        else
        {
            section[i].index = i;
            section[i].start = atoi(argv[1])/THREAD_NUM*i; 
            section[i].end = atoi(argv[1]);
        } 
    }

    //pthread_t tid[3]
    pthread_t tid[THREAD_NUM]; /* thread identifiers for child threads  */
    pthread_attr_t attr; /* thread attribute  */
    
    pthread_attr_init(&attr); /* initialize thread attribute variable with default attributes */

    for (i=0; i<THREAD_NUM; i++) /* create child threads */
    {
        //int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);
        //pthread_t* thread: The ID of the created thread is stored here after creation of thread
        pthread_create(&tid[i], &attr, runner, &section[i]);
    }

    for (i=0; i<THREAD_NUM; i++) /* wait for child threads to exit */
    {
        //int pthread_join(pthread_t thread, void **value_ptr); waits until thread has finished executing
        pthread_join(tid[i], NULL);
        sum += sum_thread[i]; /* add the summation result obtained from each child thread to overall summation */
    }

    printf("sum = %d\n", sum);

    return 0;
}

//Anything within this is thread-specific, since Stack is thread-specific. Thread-specific: Stack (local var's) and Heap
//(memory allocated with malloc() ). Shared between threads: Data (local var's) and Text (program code).
void *runner(void *param) //Anything within this is thread-specific, since Stack is thread-specific
{
    //local var's, thread-specific
    int i;
    int index, start, end;

    struct Section *section;
    section = param; //Data type of param: &Section, so requires Section*

    index = section->index;
    start = section->start;
    end = section->end;

    sum_thread[index] = 0;

    for (i=start; i<=end; i++)
        sum_thread[index] += i;

    pthread_exit(0);
}
