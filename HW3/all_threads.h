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
		  
****************************************************************/
void *masterService(void *tinfo);
void *child_1Service(void *tinfo);
void *child_2Service(void *tinfo);


#define NUM_CHAR 3	//macro used to log letters with exactly 3 occurences in the
			//file valentinesday.txt
