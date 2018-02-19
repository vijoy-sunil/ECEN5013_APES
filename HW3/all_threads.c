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
#include "main.h"
#include "all_threads.h"
#include "my_timer.h"
#include "doubly_ll.h"

bool continue_ch1 = true;
bool continue_ch2 = true;

																//signal handler to set boolean used
																// to exit child threads
void ch_exit(int signum)
{
	if(signum == SIGUSR1)
		continue_ch1 = false;
	else if(signum == SIGUSR2)
		continue_ch2 = false;
}

																//masterService thread
void *masterService(void *tinfo)
{
	struct timeval time_stamp;
	gettimeofday(&time_stamp,NULL);												//log time when this thread starts

	//----------------------------------------entry logging------------------------------------------------------------
	pthread_mutex_lock(&logfile_lock);
	static FILE *logfile_ptr;												//open file
	threadinfo_t* masterStruct = (threadinfo_t*)tinfo;
	logfile_ptr = fopen(masterStruct->logfile, "a+");
	if(logfile_ptr == NULL)
	{
		printf("Unable to open logfile - masterStruct\n");
		return NULL;
	}
	
																//write string identifier,
																//write pid, tid, time
	fprintf(logfile_ptr, "RUNNING: mainService thread\n");
	fprintf(logfile_ptr, "POSIXID: %lu\n", pthread_self());
	fprintf(logfile_ptr, "LINUXID: %d\n", getpid());
	fprintf(logfile_ptr, "TIMESTAMP: mainService thread start %ld.%ld\n",time_stamp.tv_sec, time_stamp.tv_usec);
	fprintf(logfile_ptr, "\n------------------------------------------------------------------- \n");
	fclose(logfile_ptr);													//close file
	pthread_mutex_unlock(&logfile_lock);
	//------------------------------------Creating child threads------------------------------------------------------
	int rc;

	rt_param[CHILD1_SERVICE].sched_priority = rt_max_prio-2;
	pthread_attr_setschedparam(&rt_sched_attr[CHILD1_SERVICE], &rt_param[CHILD1_SERVICE]);

	printf("\nSpawning child_1Service thread\n");
	rc = pthread_create(&threads[CHILD1_SERVICE], &rt_sched_attr[CHILD1_SERVICE], child_1Service, (void *)tinfo);

	if (rc)
	{	
		printf("ERROR: pthread_create() rc is %d\n", rc);
		perror(NULL);
		exit(-1);
	}
	


	rt_param[CHILD2_SERVICE].sched_priority = rt_max_prio-3;
	pthread_attr_setschedparam(&rt_sched_attr[CHILD2_SERVICE], &rt_param[CHILD2_SERVICE]);

	printf("\nSpawning child_2Service thread\n");
	rc = pthread_create(&threads[CHILD2_SERVICE], &rt_sched_attr[CHILD2_SERVICE], child_2Service, (void *)tinfo);

	if (rc)
	{
		printf("ERROR: pthread_create() rc is %d\n", rc);
		perror(NULL);
		exit(-1);
	}
	
	//----------------------------------------joining children---------------------------------------------------------
	if(pthread_join(threads[CHILD1_SERVICE], NULL) == 0)
		printf("\nchild_1Service JOINED\n");
	else
		perror("child_1Service\n");

	if(pthread_join(threads[CHILD2_SERVICE], NULL) == 0)
		printf("\nchild_2Service JOINED\n");
	else
		perror("child_2Service\n");

	//-----------------------------------------exit logging------------------------------------------------------------
	logfile_ptr = fopen(masterStruct->logfile, "a+");
	gettimeofday(&time_stamp,NULL);
	fprintf(logfile_ptr,"TIMESTAMP: mainService thread end %ld.%ld\n", time_stamp.tv_sec, time_stamp.tv_usec);
	fprintf(logfile_ptr, "\n------------------------------------------------------------------- \n");
	fclose(logfile_ptr);	

	printf("\nmasterService EXITING\n");
  	pthread_exit(NULL);
}

void *child_1Service(void *tinfo)
{
	FILE* valentinesday_ptr;
	struct timeval time_stamp;
	gettimeofday(&time_stamp,NULL);												//log time when this thread starts

	//----------------------------------------entry logging-------------------------------------------------------------
	pthread_mutex_lock(&logfile_lock);
	static FILE *logfile_ptr;												//open file
	threadinfo_t* child1_Struct = (threadinfo_t*)tinfo;
	logfile_ptr = fopen(child1_Struct->logfile, "a+");
	if(logfile_ptr == NULL)
	{
		printf("ERROR: Unable to open logfile - child1_Struct\n");
		fclose(logfile_ptr);
		return NULL;
	}

	fprintf(logfile_ptr, "RUNNING: child1_Service thread\n");
	fprintf(logfile_ptr, "POSIXID: %lu\n", pthread_self());
	fprintf(logfile_ptr, "LINUXID: %d\n", getpid());
	fprintf(logfile_ptr, "TIMESTAMP: child1_Service thread start %ld.%ld\n",time_stamp.tv_sec, time_stamp.tv_usec);
	fprintf(logfile_ptr, "\n------------------------------------------------------------------- \n");

	fclose(logfile_ptr);													//close file
	pthread_mutex_unlock(&logfile_lock);
	//------------------------------------linked list search-----------------------------------------------------------
	signal(SIGUSR1, ch_exit);
	
	valentinesday_ptr = fopen("valentinesday.txt", "r");
	if(valentinesday_ptr == NULL)
	{
		printf("ERROR: Unable to open valentinesday.txt - child1_Struct\n");
		fclose(valentinesday_ptr);
		return NULL;
	}
	
	node_t* head;
	node_t* anode;
	info_t* node_addr;
	char found = 0;
	int letter;
	letter = fgetc(valentinesday_ptr);
	if(letter > 64 && letter < 91)												//convert to lower case
		letter = letter + 32;

	anode = insert_at_end(NULL, (char)letter);										//create head of the list
	node_addr = GET_LIST_CONTAINER(anode, info_t, list);
	node_addr->count = 1;
	head = anode;	
	
	
	while(1){
		letter = fgetc(valentinesday_ptr);
		if(letter > 64 && letter < 91)											//convert to lower case
			letter = letter + 32;

		if(letter == EOF)
			break;
		else
		{
			while(found == 0)
			{			
				if(letter == node_addr->data)
				{
					node_addr->count++;
					found = 1;	
				}
				else if(anode->next == NULL)
				{
					anode = insert_at_end(head, (char)letter);
					while(anode->next != NULL)			
						anode = anode->next;								//goto end
					node_addr = GET_LIST_CONTAINER(anode, info_t, list);					//get address of last node		
					node_addr->count++;
					found = 1;
				}
				else
				{	
					anode = anode->next;
					node_addr = GET_LIST_CONTAINER(anode, info_t, list);
				}
			}
		}

		anode = head;													//reset pointer back to head
		node_addr = GET_LIST_CONTAINER(anode, info_t, list);
		found = 0;	
	}
	fclose(valentinesday_ptr);
	//print_mylist(head);

	pthread_mutex_lock(&logfile_lock);
	logfile_ptr = fopen(child1_Struct->logfile, "a+");

	fprintf(logfile_ptr,"Characters with only 3 occurences in valentinesday.txt file\n");
	anode = head;
	while(anode != NULL)													//character with only 3 occurences
	{
		node_addr = GET_LIST_CONTAINER(anode, info_t, list);
		if(node_addr->count == NUM_CHAR)
			fprintf(logfile_ptr,"%c\n", node_addr->data);
		anode = anode->next;
	}
	fprintf(logfile_ptr, "\n------------------------------------------------------------------- \n");
	fclose(logfile_ptr);	
	pthread_mutex_unlock(&logfile_lock);

	while(continue_ch1)	sleep(1);											//"continue_ch1" bool will be unset through user signal USR1
	//-----------------------------------------exit logging------------------------------------------------------------
	pthread_mutex_lock(&logfile_lock);
	logfile_ptr = fopen(child1_Struct->logfile, "a+");
	fprintf(logfile_ptr,"TIMESTAMP: child1_Service thread end %ld.%ld\n", time_stamp.tv_sec, time_stamp.tv_usec);
	fprintf(logfile_ptr, "\n------------------------------------------------------------------- \n");
	fclose(logfile_ptr);	
	pthread_mutex_unlock(&logfile_lock);

	printf("\nchild1_Service EXITING\n");
	pthread_exit(NULL);
}

void *child_2Service(void *tinfo)
{

	struct timeval time_stamp;
	gettimeofday(&time_stamp,NULL);												//log time when this thread starts

	//----------------------------------------entry logging------------------------------------------------------------
	pthread_mutex_lock(&logfile_lock);
	static FILE *logfile_ptr;												//open file
	threadinfo_t* child2_Struct = (threadinfo_t*)tinfo;
	logfile_ptr = fopen(child2_Struct->logfile, "a+");
	if(logfile_ptr == NULL)
	{
		printf("ERROR: Unable to open logfile - child2_Struct\n");
		fclose(logfile_ptr);
		return NULL;
	}

	fprintf(logfile_ptr, "RUNNING: child2_Service thread\n");
	fprintf(logfile_ptr, "POSIXID: %lu\n", pthread_self());
	fprintf(logfile_ptr, "LINUXID: %d\n", getpid());
	fprintf(logfile_ptr, "TIMESTAMP: child2_Service thread start %ld.%ld\n",time_stamp.tv_sec, time_stamp.tv_usec);
	fprintf(logfile_ptr, "\n------------------------------------------------------------------- \n");

	fclose(logfile_ptr);													//close file
	pthread_mutex_unlock(&logfile_lock);
	//------------------------------------report CPU utilization----------------------------------------------------------
	signal(SIGUSR2, ch_exit);
	unsigned long period_ns = 100000000;											//set timer interval 100 ms (1*10^8 ns)
	init_timer(period_ns, tinfo);
	
	while(continue_ch2)	sleep(1);											//"continue_ch2" bool will be unset through user signal USR2

	init_timer(0, tinfo);													//stop the timer - set period to zero

	//-----------------------------------------exit logging---------------------------------------------------------------
	pthread_mutex_lock(&logfile_lock);
	logfile_ptr = fopen(child2_Struct->logfile, "a+");
	fprintf(logfile_ptr,"TIMESTAMP: child2_Service thread end %ld.%ld\n", time_stamp.tv_sec, time_stamp.tv_usec);
	fprintf(logfile_ptr, "\n------------------------------------------------------------------- \n");
	fclose(logfile_ptr);	
	pthread_mutex_unlock(&logfile_lock);
	
	printf("\nchild2_Service EXITING\n");
	pthread_exit(NULL);
}




