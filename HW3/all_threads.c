#include "main.h"
#include "all_threads.h"
#include "my_timer.h"

bool continue_ch1 = true;
bool continue_ch2 = true;
void ch_exit(int signum)
{
	if(signum == SIGUSR1)
		continue_ch1 = false;
	else if(signum == SIGUSR2)
		continue_ch2 = false;
}

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

	fclose(logfile_ptr);													//close file
	pthread_mutex_unlock(&logfile_lock);
	//------------------------------------linked list search-----------------------------------------------------------
	signal(SIGUSR1, ch_exit);
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

	fclose(logfile_ptr);													//close file
	pthread_mutex_unlock(&logfile_lock);
	//------------------------------------report CPU utilization----------------------------------------------------------
	signal(SIGUSR2, ch_exit);
	unsigned long period_ns = 100000000;											//set timer interval 100 ms (1*10^8 ns)
	init_timer(period_ns, tinfo);
	
	while(continue_ch2)	sleep(1);											//"continue_ch2" bool will be unset through user signal USR2

	//stop the timer

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




