//Name: Jonas Vinter-Jensen
//SFSU ID: 912941515
//Homework #1 / CSC 415 [01] / Fall-17

#include <stdio.h>
#include <unistd.h>

#define NAME "JonasVJ"

int main()
{
	//Allocate 80 characters max for the output string 	
    char buffer[80];
	int standard_out = 1;
	size_t outsize = 80; //Allow output with same limit as buffer max 
	
	sprintf(buffer, "Hello CSC415, I am %s\n", NAME); //Concatenate string literal and NAME into buffer
	write(standard_out, buffer, outsize); //Output buffer to stdout with 80 char's as maximum

	return 0; //0: EXIT_SUCCESS 
}
