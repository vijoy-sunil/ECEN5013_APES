#include "includes.h"
#include "threads.h"
#include "signals.h"
#include "messageQue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mqueue.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "errorhandling.h"

sig_atomic_t light_heartbeat_flag;
sig_atomic_t temperature_heartbeat_flag;
sig_atomic_t logger_heartbeat_flag;

#define SLEEP(t)      struct timespec current,remaining; \
        current.tv_nsec = 0; current.tv_sec = t; \
        do { \
                ret = nanosleep(&current,&remaining); \
                if(ret == -1) { \
                        current.tv_sec = remaining.tv_sec; \
                        current.tv_nsec = remaining.tv_nsec;} \
        } while(ret != 0);  \

void LightHBhandler(int sig){
        if(sig == LIGHT_HB_SIG)
        {
                //  printf("L");
                light_heartbeat_flag = 1;
        }
}

void TempHBhandler(int sig){
        if(sig == TEMPERATURE_HB_SIG)
        {
                //  printf("T");
                temperature_heartbeat_flag = 1;
        }
}

void LogHBhandler(int sig){
        if(sig == LOGGER_HB_SIG)
        {
                //    printf("l");
                logger_heartbeat_flag = 1;
        }
}

int main()
{
        int ret;
        message_packet = (err_message_packet*)malloc(sizeof(err_message_packet));
        if(message_packet == NULL) {perror("malloc-main"); return -1;}

        printf("Entering Main\n");
        gclose_app = 1; gclose_light = 1; gclose_temp = 1; gclose_log = 1;
        temperature_heartbeat_flag = 0; light_heartbeat_flag = 0;
/**************install SIGINT handler to close application through ctrl + c*************/
        signal(SIGINT,SIGINT_handler);

/******initialize error reporting msgq*************************************/
        mqd_t messagequeue_error;
        int msg_prio_err = MSG_PRIO_ERR;
        int num_bytes_err;
        struct mq_attr messagequeue_attr_error = {.mq_maxmsg = MAX_QUE_MSGSIZE, //max # msg in queue
                                        .mq_msgsize = BUFFER_SIZE,//max size of msg in bytes
                                        .mq_flags = 0};

        messagequeue_error = mq_open(MY_MQ_ERR, //name
                           O_CREAT | O_RDWR,//flags. create a new if dosent already exist
                           S_IRWXU, //mode-read,write and execute permission
                           &messagequeue_attr_error); //attribute
        if(messagequeue_error < 0) {perror("mq_open-error_mq-tempTask "); return -1;}
        else printf("Messgae Que Opened in tempTask\n");
//set up notification for this error

        sig_ev_err.sigev_notify=SIGEV_THREAD;      //notify by signal in sigev_signo
        sig_ev_err.sigev_notify_function = errorFunction;
        sig_ev_err.sigev_notify_attributes=NULL;
        sig_ev_err.sigev_value.sival_ptr=&messagequeue_error;  //data passed with notification


        ret  = mq_notify(messagequeue_error,&sig_ev_err);
        if(ret == -1) {perror("mq_notify-main"); return -1;}

// //set up handler
//         struct sigaction action;
//         sigemptyset(&action.sa_mask);
//         action.sa_handler = LogQNotifyhandler;
//         ret = sigaction(SIGLOG,&action,NULL);
//         if(ret == -1) { perror("sigaction temptask"); return NULL; }
//         printf("pid:%d\n",getpid());


/*******************Masking SIgnals***********************/
        sigset_t mask; //set of signals
        sigemptyset(&mask);
        sigaddset(&mask,SIGTEMP); sigaddset(&mask,SIGLIGHT);
        sigaddset(&mask,SIGTEMP_IPC); sigaddset(&mask,SIGLIGHT_IPC);
        sigaddset(&mask,SIGLOG);

        //read the status of global variables when wake up from sleep
        //sigaddset(&mask,LOGGER_HB_SIG); sigaddset(&mask,TEMPERATURE_HB_SIG);

        ret = pthread_sigmask(
                SIG_SETMASK, //block the signals in the set argument
                &mask, //set argument has list of blocked signals
                NULL); //if non NULL prev val of signal mask stored here
        if(ret == -1) { printf("Error:%s\n",strerror(errno)); return -1; }

        pthread_t temp,light,log;
        threadInfo temp_info; temp_info.thread_id = 1; temp_info.main=pthread_self();
        threadInfo light_info; light_info.thread_id = 2; light_info.main=pthread_self();
        threadInfo log_info; log_info.thread_id = 3; log_info.main=pthread_self();

//Register Light HB signal
        struct sigaction action;
        sigemptyset(&action.sa_mask);
        action.sa_handler = LightHBhandler;
        ret = sigaction(LIGHT_HB_SIG,&action,NULL);
        if(ret == -1) { perror("sigaction main"); return -1; }

//Register Temp HB signal
        sigemptyset(&action.sa_mask);
        action.sa_handler = TempHBhandler;
        ret = sigaction(TEMPERATURE_HB_SIG,&action,NULL);
        if(ret == -1) { perror("sigaction main"); return -1; }

//Register Log HB signal
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        action.sa_handler = LogHBhandler;
        ret = sigaction(LOGGER_HB_SIG,&action,NULL);
        if(ret == -1) { perror("sigaction main"); return -1; }



        ret = pthread_create(  &temp,
                               DEFAULT_THREAD_ATTR,
                               tempTask,
                               (void *)&(temp_info) );
        if (ret != 0) {  printf("Error:%s\n",strerror(errno)); return -1;}

        ret = pthread_create(  &light,
                               DEFAULT_THREAD_ATTR,
                               lightTask,
                               (void *)&(light_info) );
        if (ret != 0) {  printf("Error:%s\n",strerror(errno)); return -1;}

        ret = pthread_create(  &log,
                               DEFAULT_THREAD_ATTR,
                               logTask,
                               (void *)&(log_info) );
        if (ret != 0) {  printf("Error:%s\n",strerror(errno)); return -1;}
        uint8_t read_bytes; char choice;
        uint8_t close_light=0; uint8_t close_temperature=0; uint8_t close_logger=0;
        SLEEP(2);//allow other threads to initialize
        printf("*************************************************************\n");
        printf(" Enter thread to close 1-temp; 2-light; 3-log; 4-application\n");
        printf("*************************************************************\n");

        while (gclose_app) {

//check HB signals

                printf("M");

                if(close_light == 0) {
                        if(light_heartbeat_flag == 0) printf("NO HB from Light Task\n");
                        else {printf("L"); light_heartbeat_flag = 0;}
                }

                if(close_temperature == 0) {
                        if(temperature_heartbeat_flag == 0) printf("NO HB from Temp Task\n");
                        else {printf("T"); temperature_heartbeat_flag = 0;}
                }

                if(close_logger == 0) {
                        if(logger_heartbeat_flag == 0) printf("NO HB from Log Task\n");
                        else {printf("l*"); logger_heartbeat_flag = 0;}
                }
                fflush(stdout);

                read_bytes=read(0,&choice,sizeof(char));
                if(read_bytes == 1) {
                        printf("\nchoice:%c\n",choice);
                        switch(choice) {
                        case '1':
                                if(close_temperature == 0) {
                                        printf("sending close signal to temp task\n");
                                        gclose_temp = 0; close_temperature = 1;
                                }
                                break;
                        case '2':
                                if(close_light == 0) {
                                        printf("sending close signal to light task\n");
                                        gclose_light = 0; close_light = 1;
                                }
                                break;
                        case '3':
                                if(close_logger == 0) {
                                        printf("sending close signal to log task\n");
                                        gclose_log = 0; close_logger = 1;
                                }
                                break;
                        case '4':
                                printf("Closing application\n");
                                gclose_temp = 0; gclose_light = 0;  gclose_log = 0;
                                //pthread_cancel(temp); pthread_cancel(light);
                                //pthread_cancel(log);
                                gclose_app = 0;
                                break;
                        }
                        read_bytes = 0;
                }

//                sleep(5)
                SLEEP(5);
        }
        pthread_join(temp, NULL);
        pthread_join(light, NULL);
        pthread_join(log, NULL);
        printf("Joined all threads\n");
        mq_close(messagequeue_error);

/*********destroy message Ques***********************/
        mq_unlink(IPC_TEMP_MQ);
        mq_unlink(IPC_LIGHT_MQ);
        mq_unlink(MY_MQ);
        mq_unlink(MY_MQ_ERR);
        free(message_packet);

        printf("Destroyed all opened Msg Ques\n");

        printf("***************Exiting***************\n");
        return 0;
}
