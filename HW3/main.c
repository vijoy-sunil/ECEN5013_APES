/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 02/18/2018
* DESCRITPTION	: The program consists of 3 pthreads - 
		  masterService, child1_service, child2_Service
		  (all_threads.c)

		  masterService creates the two child services.
 		  child1_Service - reads in valentinesday.txt
		  file and uses linkedlist to search through and
		  track number of occurences of alphabets.

		  Finally, logs those alphabets with exactly 3
		  occurences.

		  child2_Service - reports CPU utilization to the
		  log file every 100 ms using a timer.

		  All 3 threads writes these into log file:
		  1. timestamp on entry and exit
		  2. thread identifiers - posix id, linux id

		  The two child threads are implemented in a way 
		  that user can exit them using USR1 and USR2 signals

		  The log file access from all 3 threads are protected
		  using mutex lock.

		  Usage: ./pthread.exe <logfile_name.txt>
		  
****************************************************************/
#include "main.h"
#include "all_threads.h"

int main (int argc, char *argv[])
{
	int rc, i, scope;
	threadinfo_t* tinfo = (threadinfo_t*)malloc(sizeof(tinfo));

	if(argc != 2)
	{
		printf("Usage: ./pthread_exe <log file name>\n");
		exit(-1);
	}
	else									//accepts logfile name from command line
	{
		tinfo->logfile = argv[1];
	}


	if(pthread_mutex_init(&logfile_lock, NULL)!= 0)
	{
		printf("ERROR:init mutex logfile_lock\n");
		return 1;
	}
										//set attributes for mainService thread
	pthread_attr_init(&rt_sched_attr[START_SERVICE]);
	pthread_attr_setinheritsched(&rt_sched_attr[START_SERVICE], PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&rt_sched_attr[START_SERVICE], SCHED_FIFO);

	rt_max_prio = sched_get_priority_max(SCHED_FIFO);
	rt_min_prio = sched_get_priority_min(SCHED_FIFO);

	rt_param[START_SERVICE].sched_priority = rt_max_prio - 1;
	pthread_attr_setschedparam(&rt_sched_attr[START_SERVICE], &rt_param[START_SERVICE]);
										//create mainService thread
	printf("\nSpawning masterService thread\n");
	rc = pthread_create(&threads[START_SERVICE], &rt_sched_attr[START_SERVICE], masterService, (void *)tinfo);

	if (rc)
	{
		printf("ERROR: pthread_create() rc is %d\n", rc);
		perror(NULL);
		exit(-1);
	}
	

	if(pthread_join(threads[START_SERVICE], NULL) == 0)
		printf("\nmasterService JOINED\n");
	else
		perror("masterService\n");

	pthread_mutex_destroy(&logfile_lock);
	exit(0);
}






