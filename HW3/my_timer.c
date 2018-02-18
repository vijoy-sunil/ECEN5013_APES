#include "main.h"

static threadinfo_t* handlerStruct;

void my_handler(union sigval arg)				//handler function for timer
{
	long unsigned int cpu_time[10];
	static long unsigned int idle_1, idle_2, diff;
	static int cnt_10 = 1;
	int ncpus;
	FILE* fstat;

	pthread_mutex_lock(&logfile_lock);
	static FILE* logfile_ptr;
	logfile_ptr = fopen(handlerStruct->logfile, "a+");				
	if(logfile_ptr == NULL){
		printf("ERROR: Unable to open logfile - timer handler(child2_Struct)\n");	
		fclose(logfile_ptr);
	}	

								//report cpu utilization
	fprintf(logfile_ptr, "CPU Utilization: logged by child2_Service thread\n");
	
	if(cnt_10 == 1)
	{
		fstat = fopen("/proc/stat","r");
		if(fstat == NULL)
			printf("ERROR: Unable to open /proc/stat - timer handler(child2_Struct)\n");

		fscanf(fstat, "%*s" "%lu" "%lu" "%lu" "%lu" "%lu" "%lu" "%lu" "%lu", &cpu_time[0],
			&cpu_time[1], &cpu_time[2], &cpu_time[3], &cpu_time[4], &cpu_time[5], &cpu_time[6],
			&cpu_time[7]);

		fclose(fstat);
		//4th number in the line is the CPU total IDLE time
		idle_1 = cpu_time[3];
	}


	cnt_10 = cnt_10 + 1;

	if(cnt_10 == 10)
	{
		cnt_10 = 1;
		fstat = fopen("/proc/stat","r");
		if(fstat == NULL)
			printf("ERROR: Unable to open /proc/stat - timer handler(child2_Struct)\n");

		fscanf(fstat, "%*s" "%lu" "%lu" "%lu" "%lu" "%lu" "%lu" "%lu" "%lu", &cpu_time[0],
			&cpu_time[1], &cpu_time[2], &cpu_time[3], &cpu_time[4], &cpu_time[5], &cpu_time[6],
			&cpu_time[7]);

		fclose(fstat);	
		//4th number in the line is the CPU total IDLE time	
		idle_2 = cpu_time[3];
		ncpus = get_nprocs();

		diff = (idle_2 - idle_1)/ncpus;
	}

	
	
	fprintf(logfile_ptr, "CPU utilization: %lu (IDLE percentage)\n", diff );	
	

	fprintf(logfile_ptr, "\n------------------------------------------------------------------- \n");
	
	fclose(logfile_ptr);					//close file
	pthread_mutex_unlock(&logfile_lock);
}


void init_timer(unsigned long period_ns, void* tinfo)
{
	timer_t timer_id;
        struct sigevent sev;		//signal event
        struct itimerspec timer_params;

	//copy shared struct to handler struct
	handlerStruct = (threadinfo_t*)tinfo;

  	//Set the sigevent structure to cause the signal to be
   	//delivered by creating a new thread.
	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_value.sival_ptr = &timer_id;
	sev.sigev_notify_function = my_handler;
	sev.sigev_notify_attributes = NULL;

	//set timer interval
	timer_params.it_value.tv_sec 	= period_ns / 1000000000;
	timer_params.it_value.tv_nsec 	= period_ns % 1000000000;
	timer_params.it_interval.tv_sec = timer_params.it_value.tv_sec;
	timer_params.it_interval.tv_nsec= timer_params.it_value.tv_nsec;

	//create timer
	if (timer_create(CLOCK_REALTIME, &sev, &timer_id) == -1)
		printf("\nERROR: timer create\n");

	else if (timer_settime(timer_id, 0, &timer_params, NULL) == -1)
                printf("\nERROR: timer start\n");	

}
