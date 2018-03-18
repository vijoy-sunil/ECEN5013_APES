#ifndef THREAH_H
#define THREAD_H

#include <signal.h>
#include <unistd.h>

#define DEFAULT_THREAD_ATTR ((void *)0)

pthread_mutex_t lock_temp ;
pthread_cond_t cond_var_temp;
sig_atomic_t temp_flag_glb;

pthread_mutex_t glight_mutex ;
pthread_cond_t glight_condition;
sig_atomic_t glight_flag;

typedef struct
{
        int t_id;
        char* plog_file;
        pthread_t main;
} threadTaskAttr;

void *LightTask(void *pthread_inf);
void *TemperatureTask(void *pthread_inf);
void *logTask(void *pthread_inf);
void *socketTask(void *pthread_inf);

#endif
