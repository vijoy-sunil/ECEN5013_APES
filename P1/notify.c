#include "notify.h"
#include "tasks.h"


int ConfigTimerTemp(){
        int rc;

        sigset_t mask; 
        sigemptyset(&mask);
        sigaddset(&mask,LIGHT_SIGNAL); sigaddset(&mask,LIGHT_HB_SIG);
        sigaddset(&mask,LOGGER_HB_SIG); sigaddset(&mask,TEMPERATURE_HB_SIG);
        sigaddset(&mask,LOGGER_SIGNAL);

        rc = pthread_sigmask(SIG_SETMASK,&mask,NULL);
        if(rc == -1) 
        { 
            printf("Error:%s\n",strerror(errno)); 
            return -1; 
        }

        struct sigaction sig_act ={.sa_flags = SA_RESTART,.sa_handler = temperature_signal};

        sig_act.sa_mask = mask;

        rc = sigaction(TEMPERATURE_SIGNAL,&sig_act,NULL);

        if(rc == -1) 
        { 
            perror("sigaction ConfigTimerTemp"); 
            return -1; 
        }

        timer_t IDTime;
        struct sigevent sig_ev ={.sigev_notify=SIGEV_SIGNAL,.sigev_signo = TEMPERATURE_SIGNAL,.sigev_value.sival_ptr=&IDTime};

        rc = timer_create(CLOCK_MONOTONIC,&sig_ev,&IDTime);

        if(rc == -1) 
        { 
            perror("timer_create ConfigTimerTemp"); 
            return -1; 
        }

//Start timer
        struct itimerspec its={.it_value.tv_sec=2,.it_value.tv_nsec=0,.it_interval.tv_sec=NSEC_FREQUENCY/1000000000,
                .it_interval.tv_nsec=NSEC_FREQUENCY%1000000000
        };

        rc = timer_settime(IDTime,0, &its, NULL);

        if(rc == -1)  
        { 
            perror("timer_settime ConfigTimerTemp"); 
            return -1; 
        }

        return 1;
}


int ConfigLightSetup(){
        int rc;
        sigset_t mask; //set of signals
        sigemptyset(&mask);
        sigaddset(&mask,TEMPERATURE_SIGNAL); sigaddset(&mask,TEMPERATURE_HB_SIG);
        sigaddset(&mask,LOGGER_SIGNAL); sigaddset(&mask,LIGHT_HB_SIG);
        sigaddset(&mask,LOGGER_HB_SIG);


        rc = pthread_sigmask(SIG_SETMASK,&mask,NULL); 

        if(rc == -1) 
        { 
            printf("Error:%s\n",strerror(errno)); 
            return -1; 
        }

        struct sigaction sig_act ={.sa_flags = SA_RESTART,  .sa_handler = light_signal};

        sig_act.sa_mask=mask;
        rc = sigaction(LIGHT_SIGNAL,&sig_act,NULL);

        if(rc == -1) 
        { 
            perror("timer_create ConfigLightSetup"); 
            return -1; 
        }

        timer_t IDTime;
        struct sigevent sig_ev ={.sigev_notify=SIGEV_SIGNAL,.sigev_signo = LIGHT_SIGNAL,.sigev_value.sival_ptr=&IDTime };

        rc = timer_create(CLOCK_MONOTONIC,&sig_ev,&IDTime);

        if(rc == -1) 
        { 
            perror("timer_create ConfigLightSetup"); 
            return -1; 
        }

        struct itimerspec its={
                .it_value.tv_sec=2, 
                .it_value.tv_nsec=0,
                .it_interval.tv_sec=NSEC_FREQUENCY/1000000000,
                .it_interval.tv_nsec=NSEC_FREQUENCY%1000000000
        };

        rc = timer_settime(IDTime,0,&its, NULL );

        if(rc == -1)  
        { 
            perror("timer_settime ConfigLightSetup"); 
            return -1; 
        }

        return 1;
}


void Signal_interrupt(int sig){
        if(sig == SIGINT) {
                app_close   = 0;
        }
}

void temperature_signal(int sig){
        pthread_mutex_lock(&temperature_mutex);
        temperature_flag = 1;
        pthread_cond_signal(&temperature_cond_var);
        pthread_mutex_unlock(&temperature_mutex);
}


void light_signal(int sig){
        pthread_mutex_lock(&light_mutex);
        light_flag = 1;
        pthread_cond_signal(&light_cond_var);
        pthread_mutex_unlock(&light_mutex);
}