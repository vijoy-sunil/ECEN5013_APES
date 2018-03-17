#include <signal.h>
#include <unistd.h>

pthread_mutex_t temperature_mutex ;
pthread_cond_t temperature_cond_var;
sig_atomic_t temperature_flag;

pthread_mutex_t light_mutex ;
pthread_cond_t light_cond_var;
sig_atomic_t light_flag;

void *lightTask(void *pthread_pckt);
void *TemperatureTask(void *pthread_pckt);
void *logTask(void *pthread_pckt);

//Generic sturcture for the threadPckt 
typedef struct
{
	pthread_t main_task;
	int t_id;
} threadPckt;

