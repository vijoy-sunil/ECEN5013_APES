/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 01/30/2018
* DESCRITPTION	: Tracking system calls and library calls with
		  file io operations using strace and ltrace.
* HEADER FILES	: file_io.h
****************************************************************/

#include "file_io.h"

int main(void)
{
	//Operation #1: Print to std out using printf
	printf("Earth is not flat!!!\n");

	//Operation #2: Creating a file,
	//Add read/write permission and open the file
	FILE *fp;
	fp = fopen("testfile.txt", "w+");
	if(fp == NULL){
		printf("Unable to open file\n");
		return 0;
	}
	printf("File created\n");

	//Operation #3: Write a character to the file
	char ch = 'Q';
	printf("Writing single character ('Q') to file\n");
	putc(ch, fp);

	//Operation #4: Close the file
	printf("File closed\n");
	fclose(fp);

	//Operation #5: Open the file in append mode
	printf("File opened in append mode\n");
	fp = fopen("testfile.txt", "a");

	//Operation #6: Dynamically allocate an array of memory
	char *buffer;
	uint8_t len = MAX_FILE_SIZE;
	buffer = malloc(len * sizeof(uint8_t));

	//Operation #7: Read an input string from the command line and write to string
	printf("Enter string to write to file:");
	scanf("%[^\n]s", buffer);
	fprintf(fp, "%s", buffer);
	fclose(fp);

	//Operation #8: Flush file output
	printf("File opened in read mode\n");
	fp = fopen("testfile.txt", "r");
	printf("Flushing file contents :");
	char file_op[MAX_FILE_SIZE];
	int i;
	for(i = 0; i < MAX_FILE_SIZE; i++)
	{
		ch = getc(fp);
		if(ch == EOF){
			printf("\nFlush complete\n");
			break;
		}
		printf("%c\t", ch);
	}
	

	//Operation #9: Close the file
	printf("File closed\n");
	fclose(fp);

	//Operation #10: Open file for reading
	printf("File opened in read mode\n");
	fp = fopen("testfile.txt", "r");

	//Operation #11: Read a single character from file
	ch = getc(fp);
	printf("Read single character from file : %c\n", ch);

	//Operation #12: Read a string of character from file
	fp = fopen("testfile.txt", "r");
	char str[MAX_FILE_SIZE];
	fgets(str, MAX_FILE_SIZE, fp);
	
	printf("Read string of characters from file :");
	puts(str);

	//Operation #13: Close the file
	printf("File closed\n");
	fclose(fp);

	//Operation #14: Free the memory
	printf("Free allocated memory\n");
	free(buffer);
	
	return 0;

}
