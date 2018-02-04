/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 01/30/2018
* DESCRITPTION	: This program does a system call that copies an 
		  array from user to kernel space, sorts it and
		  copies it back to user space
		  
* HEADER FILES	: call_syscall_sorter.h
****************************************************************/

#include "call_syscall_sorter.h"

int main()
{
	int32_t *input_buffer;
	int32_t *output_buffer;

	input_buffer = malloc(BUFFER_DEPTH * sizeof(uint32_t));
	output_buffer = malloc(BUFFER_DEPTH * sizeof(uint32_t));

	// Use current time as seed for random generator
	srand (time(NULL)); 

	for(int i=0; i< BUFFER_DEPTH; i++)
		input_buffer[i] = rand() % BUFFER_DEPTH;
	
	// Call system call 5 times
	for (int i =0; i< COUNT; i++)
	{
		syscall(333, input_buffer, output_buffer, BUFFER_DEPTH);
	}

	printf("\nBefore sorting \n");
	for(int i=0; i< BUFFER_DEPTH; i++)
		printf("%ld : %ld\n", (long)i, (long)input_buffer[i]);
	
	printf("\nAfter sorting \n");
	for(int i=0; i< BUFFER_DEPTH; i++)
		printf("%ld : %ld\n", (long)i, (long)output_buffer[i]);
	
	free(input_buffer);
	free(output_buffer);
	return 0;
}
