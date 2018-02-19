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
#include <pthread.h>
#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/signal.h>
#include <stdbool.h>
#include <sys/sysinfo.h>

#define NUM_THREADS		3
				//priority of threads
#define START_SERVICE 		0
#define CHILD1_SERVICE 		1
#define CHILD2_SERVICE 		2

pthread_t threads[NUM_THREADS];
pthread_attr_t rt_sched_attr[NUM_THREADS];
pthread_mutex_t logfile_lock;

int rt_max_prio, rt_min_prio;

struct sched_param rt_param[NUM_THREADS];

				//Shared structure between 3 threads
typedef struct threadinfo
{
	pthread_t pid;		//posix thread if
	pid_t tid;		//linux thread id
	char* logfile;		//name of log file

}threadinfo_t;
