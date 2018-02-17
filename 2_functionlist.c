#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SIZE 1024

int DONE = 0; //Global: needs to be available in main() and respond() 

void respond(int answer);

int main()
{
	char input[SIZE]; //1024 byte buffer
	int answer = 0;
	
	while(DONE==0)
	{
		printf("\n1. Hello World!\n2. List files\n3. Exit\nPlease select: ");

		//Read 1 character from terminal into input
		if(fgets(input, SIZE, stdin) != NULL) 
		{
            answer = atoi(input); //Convert input string to integer
            
			if(answer != 0) 
			{
				respond(answer);
			}
			else {perror("Input not a digit");}
		}
		else //No input was received and given
		{perror("Input must be either '1', '2' or '3')");} 
	}

	printf("\nExiting...\n");
	return 0;
}

void respond(int answer)
{
	if (answer==1)
	{
		pid_t pid1;
		pid1 = fork(); //Create new child process starting execution from this line
		if(pid1==0) //Will only be executed in child
		{
			printf("\nHello World!");
			exit(EXIT_SUCCESS);
		}
        else if(pid1<0)
        {
            fprintf(stderr, "Fork failed");
        }
		else //Executed only in parent
		{
			//Null: do not store child status information in wait parameter
			if(wait(NULL)==-1)
			{
				perror("wait() child termination error");
			}  
		}
	}
	else if (answer==2)
	{
		pid_t pid2; 
		pid2 = fork(); //Create new child process starting execution from this line
		if(pid2==0) //Will only be executed in child
		{
			execlp("/bin/ls", "ls", NULL);
			//exit(EXIT_SUCCESS); Not needed, since /bin/ls ends with return 0
		}
        else if(pid2<0)
        {
            fprintf(stderr, "Fork failed");
        }
		else //Executed only in parent
		{
			//Null: do not store child status information in wait parameter
			if(wait(NULL)==-1)
			{
				perror("wait() child termination error");
			}  
		}
	}
	else if (answer==3)
	{
		DONE = 1; 
	}
	else
	{
		printf("Invalid choice!\n");
	}

	return;
}



