#include <pthread.h>
#include <stdio.h>
#include <sched.h>
#include <stdlib.h>

#define NUM_THREADS		3
#define START_SERVICE 		0
#define CHILD1_SERVICE 		1
#define CHILD2_SERVICE 		2

pthread_t threads[NUM_THREADS];
pthread_attr_t rt_sched_attr[NUM_THREADS];
int rt_max_prio, rt_min_prio;

struct sched_param rt_param[NUM_THREADS];

//Shared structure between 3 threads
typedef struct threadinfo
{
	pthread_t pid;		//posix thread if
	pid_t tid;		//linux thread id
	char logfile[15];	//name of log file

}threadinfo_t;
