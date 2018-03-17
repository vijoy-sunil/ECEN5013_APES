#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include "tasks.h"
#include "notify.h"
#include "queue.h"
#include <mqueue.h>
#include "packetdefinition.h"
#include "errorhandling.h"

#include "adps9301.h"

#define NSEC_FREQUENCY (1000000000)

sig_atomic_t light_IPC_flag;

void LightIPChandler(int sig){
        if(sig == LIGHT_SIGNAL_IPC)
        {printf("Caught signal LightIPChandler\n");
         light_IPC_flag = 1;}
}

void *lightTask(void *pthread_inf) {
        light_IPC_flag = 0;
        int rc;
        threadPckt *ppthread_info = (threadPckt *)pthread_inf;

/*****************Mask SIGNALS********************/
        sigset_t mask; //set of signals
        sigemptyset(&mask); sigaddset(&mask,TEMPERATURE_SIGNAL); sigaddset(&mask,LIGHT_HB_SIG);
        sigaddset(&mask,LOGGER_HB_SIG); sigaddset(&mask,TEMPERATURE_HB_SIG); sigaddset(&mask,LOGGER_SIGNAL);

        rc = pthread_sigmask(
                SIG_SETMASK, //block the signals in the set argument
                &mask, //set argument has list of blocked signals
                NULL); //if non NULL prev val of signal mask stored here
        if(rc == -1) { printf("Error:%s\n",strerror(errno)); return NULL; }

/*******Initialize ERROR Message Que*****************/
        mqd_t messagequeue_error;
        int msg_prio_err = MESSAGE_PRIORITY_ERR;
        int num_bytes_err;
        struct mq_attr messagequeue_attr_error = {.mq_maxmsg = MAX_QUE_MSGSIZE, //max # msg in queue
                                        .mq_msgsize = BUFFER_SIZE,//max size of msg in bytes
                                        .mq_flags = 0};

        messagequeue_error = mq_open(MESSAGE_Q_ERR, //name
                           O_CREAT | O_RDWR,//flags. create a new if dosent already exist
                           S_IRWXU, //mode-read,write and execute permission
                           &messagequeue_attr_error); //attribute
        if(messagequeue_error < 0) {perror("mq_open-error_mq-tempTask "); return NULL;}
        else printf("Messgae Que Opened in tempTask\n");



/********set periodic timer**********/
        rc = ConfigLightSetup();
        if(rc ==-1) return NULL;
        else printf("Periodic Timer set for Light Task\n");

        rc = pthread_mutex_init(&light_mutex,NULL);
        if(rc == -1) { printf("Error:%s\n",strerror(errno)); return NULL; }

/*******Initialize Logger Message Que*****************/
        mqd_t msgq;
        int msg_prio = MESSAGE_PRIORITY;
        int num_bytes;
        char message[BUFFER_SIZE];
        struct mq_attr msgq_attr = {.mq_maxmsg = MAX_QUE_MSGSIZE, //max # msg in queue
                                    .mq_msgsize = BUFFER_SIZE,//max size of msg in bytes
                                    .mq_flags = 0};

        msgq = mq_open(MESSAGE_Q, //name
                       O_CREAT | O_RDWR,//flags. create a new if dosent already exist
                       S_IRWXU, //mode-read,write and execute permission
                       &msgq_attr); //attribute
        if(msgq < 0) {perror("mq_open-lightTask Error:"); return NULL;}
        else printf("Messgae Que Opened in lightTask\n");

/***************setting msgq for IPC data Request******************/
        mqd_t IPCmsgq;
        int IPCmsg_prio = 20;
        int IPCnum_bytes;

        struct mq_attr IPCmsgq_attr = {.mq_maxmsg = MAX_QUE_MSGSIZE, //max # msg in queue
                                       .mq_msgsize = BUFFER_SIZE, //max size of msg in bytes
                                       .mq_flags = 0};

        IPCmsgq = mq_open(LIGHT_MESSAGE_Q, //name
                          O_CREAT | O_RDWR, //flags. create a new if dosent already exist
                          S_IRWXU, //mode-read,write and execute permission
                          &IPCmsgq_attr); //attribute
        if(IPCmsgq < 0) {perror("mq_open-lightTask Error:"); return NULL;}
        else printf("IPC Messgae Que Opened in lightTask\n");

//set up the signal to request data
        struct sigaction action;
        sigemptyset(&action.sa_mask);
        action.sa_handler = LightIPChandler;
        rc = sigaction(LIGHT_SIGNAL_IPC,&action,NULL);
        if(rc == -1) { perror("sigaction lightTask"); return NULL; }
        printf("pid:%d\n",getpid());

/************Creating logpacket*******************/
        log_pack light_log ={.log_level=1,.log_source = LightTask};
        struct timespec now,expire;

/************Init light sensor********************/
        int light = initialize_light();
        char lightbuffer[1];
        command_reg(light,CONTROL,WRITE);
        control_reg(light,WRITE,ENABLE,lightbuffer);

        float lumen;
        char data_lumen_str[8];
        uint16_t ch0,ch1;
/****************Do this periodically*******************************/
        while(light_close & app_close) {

                pthread_kill(ppthread_info->main_task,LIGHT_HB_SIG);//send HB

                pthread_mutex_lock(&light_mutex);
                while(light_flag == 0) {
                        pthread_cond_wait(&light_cond_var,&light_mutex);
                }
                pthread_mutex_unlock(&light_mutex);
                light_flag = 0;
/*************collect data*****************/
                ch0=adc_data_read(light,0);
                ch1=adc_data_read(light,1);
                lumen = report_lumen(ch0, ch1);

/************populate the log packet*********/
                time_t t = time(NULL); struct tm *tm = localtime(&t);
                strcpy(light_log.time_stamp, asctime(tm));
                sprintf(data_lumen_str,"lumen %f", lumen);
                strcpy(light_log.log_msg, data_lumen_str);
/************Log messages on Que*************/
                //  sleep(30);
                clock_gettime(CLOCK_MONOTONIC,&now);
                expire.tv_sec = now.tv_sec+2;
                expire.tv_nsec = now.tv_nsec;
                num_bytes = mq_timedsend(msgq,
                                         (const char*)&light_log,
                                         sizeof(log_pack),
                                         msg_prio,
                                         &expire);
                if(num_bytes<0) {ErrorHandler("mq_send-Log Q-lightTask",messagequeue_error,msgq,error);}
/******Log data on IPC Que if requested******/

                if(light_IPC_flag == 1) {
                        light_IPC_flag = 0;
//set up time for timed send

                        clock_gettime(CLOCK_MONOTONIC,&now);
                        expire.tv_sec = now.tv_sec+2;
                        expire.tv_nsec = now.tv_nsec;
                        num_bytes = mq_timedsend(IPCmsgq,
                                                 (const char*)&light_log,
                                                 sizeof(log_pack),
                                                 IPCmsg_prio,
                                                 &expire);
                        if(num_bytes<0) {ErrorHandler("mq_send-IPC-lightTask Error",messagequeue_error,msgq,error);}
                        else printf("data put on IPC msg Q\n");
                }


        }
        printf("exiting light task\n");
        mq_close(msgq);
        mq_close(messagequeue_error);
        mq_close(IPCmsgq);
        return NULL;

}
