#include "main.h"
#include "all_threads.h"
#include "time.h"

int delta_t(struct timespec *stop, struct timespec *start, struct timespec *delta_t);

void *masterService(void *tinfo)
{
	struct timespec start_time  = {0, 0};
	struct timespec finish_time = {0, 0};
	struct timespec thread_dt   = {0, 0};
	clock_gettime(CLOCK_REALTIME, &start_time);											//log time when this thread starts

	int rc;

	rt_param[CHILD1_SERVICE].sched_priority = rt_max_prio-2;
	pthread_attr_setschedparam(&rt_sched_attr[CHILD1_SERVICE], &rt_param[CHILD1_SERVICE]);

	printf("\nSpawning child_1Service thread\n");
	rc = pthread_create(&threads[CHILD1_SERVICE], &rt_sched_attr[CHILD1_SERVICE], child_1Service, (void *)&(tinfo));

	if (rc)
	{	
		printf("ERROR; pthread_create() rc is %d\n", rc);
		perror(NULL);
		exit(-1);
	}
	


	rt_param[CHILD2_SERVICE].sched_priority = rt_max_prio-3;
	pthread_attr_setschedparam(&rt_sched_attr[CHILD2_SERVICE], &rt_param[CHILD2_SERVICE]);

	printf("\nSpawning child_2Service thread\n");
	rc = pthread_create(&threads[CHILD2_SERVICE], &rt_sched_attr[CHILD2_SERVICE], child_2Service, (void *)&(tinfo));

	if (rc)
	{
		printf("ERROR; pthread_create() rc is %d\n", rc);
		perror(NULL);
		exit(-1);
	}
	

	if(pthread_join(threads[CHILD1_SERVICE], NULL) == 0)
		printf("child_1Service done\n");
	else
		perror("child_1Service\n");

	if(pthread_join(threads[CHILD2_SERVICE], NULL) == 0)
		printf("child_2Service done\n");
	else
		perror("child_2Service\n");

	clock_gettime(CLOCK_REALTIME, &finish_time);											//log time when this thread ends		
	delta_t(&finish_time, &start_time, &thread_dt);
 	printf("TIME: masterService thread duration: %ld sec, %ld nsec\n", thread_dt.tv_sec, thread_dt.tv_nsec);				//log duration of run
  	pthread_exit(NULL);
}

void *child_1Service(void *tinfo)
{
	struct timespec start_time  = {0, 0};
	struct timespec finish_time = {0, 0};
	struct timespec thread_dt   = {0, 0};
	clock_gettime(CLOCK_REALTIME, &start_time);											//log time when this thread starts

	printf("\nRunning child_1Service thread\n");

	clock_gettime(CLOCK_REALTIME, &finish_time);											//log time when this thread ends		
	delta_t(&finish_time, &start_time, &thread_dt);
 	printf("TIME: child_1Service thread duration: %ld sec, %ld nsec\n", thread_dt.tv_sec, thread_dt.tv_nsec);				//log duration of run
	pthread_exit(NULL);
}

void *child_2Service(void *tinfo)
{
	struct timespec start_time  = {0, 0};
	struct timespec finish_time = {0, 0};
	struct timespec thread_dt   = {0, 0};
	clock_gettime(CLOCK_REALTIME, &start_time);											//log time when this thread starts

	printf("\nRunning child_2Service thread\n");

	clock_gettime(CLOCK_REALTIME, &finish_time);											//log time when this thread ends		
	delta_t(&finish_time, &start_time, &thread_dt);
 	printf("TIME: child_2Service thread duration: %ld sec, %ld nsec\n", thread_dt.tv_sec, thread_dt.tv_nsec);				//log duration of run
	pthread_exit(NULL);
}

int delta_t(struct timespec *stop, struct timespec *start, struct timespec *delta_t)
{
  int dt_sec=stop->tv_sec - start->tv_sec;
  int dt_nsec=stop->tv_nsec - start->tv_nsec;

  if(dt_sec >= 0)
  {
    if(dt_nsec >= 0)
    {
      delta_t->tv_sec=dt_sec;
      delta_t->tv_nsec=dt_nsec;
    }
    else
    {
      delta_t->tv_sec=dt_sec-1;
      delta_t->tv_nsec=NSEC_PER_SEC+dt_nsec;
    }
  }
  else
  {
    if(dt_nsec >= 0)
    {
      delta_t->tv_sec=dt_sec;
      delta_t->tv_nsec=dt_nsec;
    }
    else
    {
      delta_t->tv_sec=dt_sec-1;
      delta_t->tv_nsec=NSEC_PER_SEC+dt_nsec;
    }
  }

  return(1);
}




