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

	//Operation #3: Write a character to the file
	char ch = 'Q';
	putc(ch, fp);

	//Operation #4: Close the file
	fclose(fp);

	//Operation #5: Open the file in append mode
	fp = fopen("testfile.txt", "a");

	//Operation #6: Dynamically allocate an array of memory
	char *buffer;
	uint8_t len = 128;
	buffer = malloc(len * sizeof(uint8_t));

	//Operation #7: Read an input string from the command line and write to string
	printf("Enter string to write to file:");
	scanf("%[^\n]s", buffer);
	fprintf(fp, "%s", buffer);

	//Operation #8: Flush file output
	fflush(fp);

	//Operation #9: Close the file
	fclose(fp);

	//Operation #10: Open file for reading
	fp = fopen("testfile.txt", "r");

	//Operation #11: Read a single character from file
	ch = getc(fp);
	printf("%c\n", ch);

	//Operation #12: Read a string of character from file
	char str[128];
	fgets(str, 20, fp);
	puts(str);

	//Operation #13: Close the file
	fclose(fp);

	//Operation #14: Free the memory
	free(buffer);
	
	return 0;

}
