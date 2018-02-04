/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 01/30/2018
* DESCRITPTION	: This program consists of a set of cron tasks
		  that will be scheduled to run every 10 minutes.
		  
* HEADER FILES	: my_crontasks.h
****************************************************************/
//			CRON syntax
/***************************************************************
# .---------------- minute (0 - 59)
# |  .------------- hour (0 - 23)
# |  |  .---------- day of month (1 - 31)
# |  |  |  .------- month (1 - 12) OR jan,feb,mar,apr ...
# |  |  |  |  .---- day of week (0 - 6) (Sunday=0 or 7) OR sun,mon,tue,wed,thu,fri,sat
# |  |  |  |  |
# *  *  *  *  * 
***************************************************************/

#include "my_crontasks.h"

int main(void)
{
	int32_t *input_buffer;
	int32_t *output_buffer;
	int pid, uid;
	char time_string[40];
	long milliseconds;

	FILE *fp;
	fp = fopen("/home/vijoy/Desktop/ECEN-5013/HW2/q4/crontasks_op.txt", "a");	
	if(fp == NULL){
		printf("Unable to open file\n");
		return 0;
	}

	input_buffer = malloc(BUFFER_DEPTH * sizeof(uint32_t));
	output_buffer = malloc(BUFFER_DEPTH * sizeof(uint32_t));

	// Use current time as seed for random generator
	srand (time(NULL)); 

	for(int i=0; i< BUFFER_DEPTH; i++)
		input_buffer[i] = rand() % BUFFER_DEPTH;

	printf("Got current date and time\n");
	gettimeofday (&tv, NULL);
	ptm = localtime (&tv.tv_sec);

	/* Format the date and time, down to a single second. */
	strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);

	/* Compute milliseconds from microseconds. */
	milliseconds = tv.tv_usec / 1000;
	fprintf(fp, "TIME OF DAY: %s.%03ld\n", time_string, milliseconds);

	printf("Got process ID\n");
	pid = getpid();
	fprintf(fp, "PROCESS ID: %d\n", pid);

	printf("Got User ID\n");
	uid = getuid();
	fprintf(fp, "USER ID: %d\n", uid);

	printf("Called sorter system call\n");
	syscall(333, input_buffer, output_buffer, BUFFER_DEPTH);
	
	fprintf(fp, "Sorter system call output:\n\n");
	fprintf(fp, "Before sorting \n");
	for(int i=0; i< BUFFER_DEPTH; i++){
		fprintf(fp, "%ld ,", (long)input_buffer[i]);
		if((i % 10 == 9) && (i != 0))
			fprintf(fp, "\n");
	}

	fprintf(fp, "\n\n");

	fprintf(fp, "After sorting \n");
	for(int i=0; i< BUFFER_DEPTH; i++){
		fprintf(fp, "%ld ,", (long)output_buffer[i]);
		if((i % 10 == 9) && (i != 0))
			fprintf(fp, "\n");
	}

	fprintf(fp, "\n---------------------------------------\n");

	fclose(fp);
	free(input_buffer);
	free(output_buffer);
	return 0;
}

