//site reference to rtes 
#include "main.h"
#include "all_threads.h"

threadinfo_t tinfo;
int main (int argc, char *argv[])
{
	int rc, i, scope;

	if(argc != 2)
	{
		printf("Usage: ./pthread_exe <log file name>\n");
		exit(-1);
	}
	else
	{
		sscanf(argv[1], "%d", &intfTime);
// here create log file
//copy name to struct
	}


	pthread_attr_init(&rt_sched_attr[START_SERVICE]);
	pthread_attr_setinheritsched(&rt_sched_attr[START_SERVICE], PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&rt_sched_attr[START_SERVICE], SCHED_FIFO);

	rt_max_prio = sched_get_priority_max(SCHED_FIFO);
	rt_min_prio = sched_get_priority_min(SCHED_FIFO);

	printf("min prio = %d, max prio = %d\n", rt_min_prio, rt_max_prio);

	rt_param[START_SERVICE].sched_priority = rt_max_prio - 1;
	pthread_attr_setschedparam(&rt_sched_attr[START_SERVICE], &rt_param[START_SERVICE]);

	printf("\nSpawning masterService thread\n");
	rc = pthread_create(&threads[START_SERVICE], &rt_sched_attr[START_SERVICE], masterService, (void *)&(tinfo));

	if (rc)
	{
		printf("ERROR; pthread_create() rc is %d\n", rc);
		perror(NULL);
		exit(-1);
	}
	

	if(pthread_join(threads[START_SERVICE], NULL) == 0)
		printf("masterService done\n");
	else
		perror("masterService\n");

	exit(0);
}






