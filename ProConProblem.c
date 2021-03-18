//Jonas Stagnaro, formerly Jonas Vinter-Jensen
//CSC 415 [01] | San Francisco State University | Fall-17
//Producer Consumer Problem using Semaphores 

//Compatability: This program will work in Ubuntu 16.04LTS 64-bit (VirtualBox), but not in OSX 
#include <pthread.h> 
#include <semaphore.h> 
#include <stdio.h>
#include <stdlib.h> //atoi() 

#define BUFFER_SIZE 16

int buffer[BUFFER_SIZE]; 
int num_consumed = 0; //Number of items that each consumer thread should consume  
int count = 0; 
int in = 0; //Contains the index of the first free slot of the buffer 
int num_of_consumers = 0;
int num_of_items = 0; //Each producer will produce this number of items   
int num_of_producers = 0; 
int out = 0; //Contains the index of the first full slot of the buffer 
int num_produced = 0; //Number of items that each producer thread will produce   

pthread_mutex_t mtx;

sem_t empty;
sem_t full;

void* consumer(void* cStruct);
void* producer(void* pStruct);

//Structs used to pass thread-specific data to each individual thread
struct Consumer_Data 
{
	int threadIndex;
};

struct Producer_Data
{
	int threadIndex; 
};

int main(int argc, const char* argv[])
{
	num_of_producers = atoi(argv[1]);
	num_of_consumers = atoi(argv[2]);
	num_of_items = atoi(argv[3]);
	printf("number of producers = %d\n", num_of_producers);
	printf("number of consumers = %d\n", num_of_consumers);
	printf("number of items/producer = %d\n\n", num_of_items);

	//Calculate the workload for each thread as the number of items they may produce/consume
	//The edge case of unequal workloads is ignored here 
	num_consumed = (num_of_items*num_of_producers)/num_of_consumers;
	num_produced = num_of_items;

   	//Initialize mutex with default attributes
   	if(pthread_mutex_init(&mtx, NULL) != 0)
   	{
        	perror("mutex initialization error");
        	printf("%s", "Exiting...");
        	exit(EXIT_FAILURE);
   	}

	//Initialize semaphore. Second parameter: pshared, which determines whether the semaphore should 
	//be shared with fork()'ed processes
	if( sem_init(&empty, 0, BUFFER_SIZE) != 0)
	{
		perror("sem_init(empty) error");
		printf("%s", "Exiting...");
		exit(EXIT_FAILURE);
	}
	if( sem_init(&full, 0, 0) != 0)
	{
		perror("sem_init(full) error");
		printf("%s", "Exiting...");
		exit(EXIT_FAILURE);
	}

   	pthread_attr_t attributes;
	pthread_attr_init(&attributes); 

	pthread_t thread_ID_C[num_of_consumers];
	pthread_t thread_ID_P[num_of_producers];

	/*Need the structs to pass the threadIndex of each thread to its thread-specific 
	function*/
	struct Consumer_Data C_Data[num_of_consumers]; 
	struct Producer_Data P_Data[num_of_producers];

	//Create consumer threads
	for(int i=0; i<num_of_consumers; i++)
	{
		C_Data[i].threadIndex = i;

		//First parameter: ID of newly created thread 
        	if( pthread_create(&thread_ID_C[i], &attributes, &consumer, &C_Data[i] ) != 0)
      	 	{
           		 perror("consumer pthread_create() error");
           		 printf("%s\n", "Exiting...");
           		 exit(EXIT_FAILURE);
       	 	}


	}
	//Create producer threads
	for(int i=0; i<num_of_producers; i++)
	{
		P_Data[i].threadIndex = i; 

		//thread_ID = ID of newly created thread
        	if( pthread_create(&thread_ID_P[i], &attributes, &producer, &P_Data[i] ) != 0)
      	 	 {
           		 perror("producer pthread_create() error");
           		 printf("%s\n", "Exiting...");
           		 exit(EXIT_FAILURE);
       	 	}
	}

	//Join consumer threads
	for(int i=0; i<num_of_consumers; i++)
	{
       	 	//Wait at this line in parent until all child threads have terminated
       		if(pthread_join(thread_ID_C[i], NULL) != 0)
       		{
            		perror("pthread_join() error");
            		printf("%s", "Exiting...");
            		exit(EXIT_FAILURE);
        	}
	}
	//Join producer threads
	for(int i=0; i<num_of_producers; i++)
	{

       		if(pthread_join(thread_ID_P[i], NULL) != 0)
       		{
            		perror("pthread_join() error");
            		printf("%s", "Exiting...");
            		exit(EXIT_FAILURE);
        	}
	}

	return 0; 
}

void* consumer(void* cStruct)
{
	int threadIndex;

	struct Consumer_Data* C_Data;

	C_Data = cStruct; 
	
	//Each thread is passed its own associated struct, so a race condition is not possible
	threadIndex = C_Data->threadIndex;

	for(int i=0; i<num_consumed; i++)
	{
		//If full==0: busy wait until some producer produces an item. 
		//else: full--
		sem_wait(&full); 

		//## Critical section start ##
		//Lock mutex to prevent multiple threads from accessing ASCII_Total at the same 
		//time
        	if( pthread_mutex_lock(&mtx) != 0 )
        	{
           		perror("Error, could not lock mutex");
           		printf("%s", "Exiting...");
            		exit(EXIT_FAILURE);
        	}

		printf("Consumer %d consumed item: %d\n", threadIndex, buffer[out]);	
		out = (out+1)%BUFFER_SIZE; //Increase out so it points to the next full slot 

		if( pthread_mutex_unlock(&mtx) != 0 )
        	{
              		perror("Error, could not unlock mutex");
            		printf("%s", "Exiting...");
           		exit(EXIT_FAILURE);
        	}
		//## Critical section end ##

		/*Empty++ happens here, and since any producer thread or any of the possible consumer 
		threads may take their turn after any sem_post(&full) or sem_post (&empty), the 
		sem_post statements must be located inside the two for loops*/
		sem_post(&empty); 
	}
 	
	pthread_exit(0);
}

void* producer(void* pStruct)
{
	//Local thread-specific variables, one copy per thread 
	int counter = 0;
	int threadIndex;

	struct Producer_Data* P_Data;
	P_Data = pStruct; 
	threadIndex = P_Data->threadIndex; 

	for(int i=0; i<num_produced; i++)
	{
		//If empty==0: busy wait until some consumer has consumed an item. 
		//Else: empty--
		sem_wait(&empty); 

		//## Critical section start ##
		//Lock mutex to prevent multiple threads from accessing ASCII_Total at the same 
		//time
        	if( pthread_mutex_lock(&mtx) != 0 )
        	{
           		perror("Error, could not lock mutex");
           		printf("%s", "Exiting...");
            		exit(EXIT_FAILURE);
        	}

		buffer[in] = threadIndex*num_produced + counter;
		in = (in+1)%BUFFER_SIZE; //Increase in so it will point to the next empty slot 

		if( pthread_mutex_unlock(&mtx) != 0 )
        	{
              		perror("Error, could not unlock mutex");
            		printf("%s", "Exiting...");
           		exit(EXIT_FAILURE);
        	}
		//## Critical section end ##

		counter++; //No need to lock this, since each thread has a local copy
		sem_post(&full); //full++ every time an item is added to buffer
	} 

	pthread_exit(0);
}


