#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "tasks.h"
#include "messageQue.h"
#include <mqueue.h>
#include "includes.h"
#include "notify.h"
#include "errorhandling.h"

// void LogQNotifyhandler(int sig){
//         if(sig == LOGGER_SIGNAL)
//                 printf("received data\n");
//
// }

sig_atomic_t log_data_flag;

void *logTask(void *pthread_inf) {
        int rc;
//log_data_flag=0;
        threadPckt *ppthread_info = (threadPckt *)pthread_inf;
/*****************Mask SIGNALS********************/
        sigset_t mask; //set of signals
        sigemptyset(&mask);
        sigaddset(&mask,LIGHT_SIGNAL); sigaddset(&mask,LIGHT_HB_SIG);
        sigaddset(&mask,LOGGER_HB_SIG); sigaddset(&mask,TEMPERATURE_HB_SIG);
        sigaddset(&mask,LOGGER_SIGNAL);

        rc = pthread_sigmask(
                SIG_SETMASK, //block the signals in the set argument
                &mask, //set argument has list of blocked signals
                NULL); //if non NULL prev val of signal mask stored here
        if(rc == -1) { printf("Error:%s\n",strerror(errno)); return NULL; }



/*******Initialize ERROR Message Que*****************/
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
        if(messagequeue_error < 0) {perror("mq_open-error_mq-tempTask "); return NULL;}
        else printf("Messgae Que Opened in tempTask\n");



/*************create a logger message q****************/
        mqd_t msgq;
        int msg_prio;
        int num_bytes;
        //char message[BUFFER_SIZE];
        log_pack* log = (log_pack*)malloc(sizeof(log_pack));
        if(log == NULL) {perror("malloc-logTask Err"); return NULL;}
        struct mq_attr msgq_attr = {.mq_maxmsg = MAX_QUE_MSGSIZE, //max # msg in queue
                                    .mq_msgsize = BUFFER_SIZE,//max size of msg in bytes
                                    .mq_flags = 0};

        msgq = mq_open(MY_MQ, //name
                       O_CREAT | O_RDWR,//flags. create a new if dosent already exist
                       S_IRWXU, //mode-read,write and execute permission
                       &msgq_attr); //attribute
        if(msgq < 0) {perror("mq_open-logTask Error:"); return NULL;}
        else printf("Messgae Que Opened in logTask\n");
//************set up notification

// //set up handler
//         struct sigaction action;
//         action.sa_mask = mask;
//         action.sa_handler = LogQNotifyhandler;
//         rc = sigaction(LOGGER_SIGNAL,&action,NULL);
//         if(rc == -1) { perror("sigaction temptask"); return NULL; }
// //        printf("pid:%d\n",getpid());
//
//         struct sigevent sig_ev ={
//                 .sigev_notify=SIGEV_SIGNAL,    //notify by signal in sigev_signo
//                 .sigev_signo = LOGGER_SIGNAL,    //Notification Signal
//                 //.sigev_value.sival_ptr=&IDTime    //data passed with notification
//         };
//
//         rc  = mq_notify(msgq,&sig_ev);
//         if(rc == -1) {perror("mq_notify-main"); return NULL;}



//open a file to write to
        FILE* pfd = fopen("logfile.txt","w");
        if(pfd==NULL) {perror("fopen-logTask Error:"); return NULL;}
        else printf("log file opened in logTask\n");


/*******&&&&&&&&&&&&&& msg Q to test IPC transfer&&&&&&&&&&&&&&&&&&**********************/
        mqd_t IPCmsgqT, IPCmsgqL;
        int IPCmsg_prio = 20;
        int IPCnum_bytes;
        char IPCmessage[BUFFER_SIZE];

        struct mq_attr IPCmsgq_attr = {.mq_maxmsg = MAX_QUE_MSGSIZE, //max # msg in queue
                                       .mq_msgsize = BUFFER_SIZE, //max size of msg in bytes
                                       .mq_flags = 0};
//temp
        IPCmsgqT = mq_open(IPC_TEMP_MQ, //name
                           O_CREAT | O_RDWR, //flags. create a new if dosent already exist
                           S_IRWXU, //mode-read,write and execute permission
                           &IPCmsgq_attr); //attribute
        if(IPCmsgqT < 0) {perror("mq_open-logTask Error:"); return NULL;}
        else printf("IPC temp Messgae Que Opened in logTask\n");
//light
        IPCmsgqL = mq_open(IPC_LIGHT_MQ, //name
                           O_CREAT | O_RDWR, //flags. create a new if dosent already exist
                           S_IRWXU, //mode-read,write and execute permission
                           &IPCmsgq_attr); //attribute
        if(IPCmsgqL < 0) {perror("mq_open-logTask Error:"); return NULL;}
        else printf("IPC light Messgae Que Opened in logTask\n");
        struct timespec now,expire;

/*******************Do this in LOOP************************************/
        while(logger_close & app_close)
        {

                pthread_kill(ppthread_info->main_task,LOGGER_HB_SIG);//send HB
//empty the message que
                do {
                        //read from queue
                        clock_gettime(CLOCK_MONOTONIC,&now);
                        expire.tv_sec = now.tv_sec+5;
                        expire.tv_nsec = now.tv_nsec;

                        num_bytes=mq_timedreceive(msgq,
                                                  (char*)log,
                                                  BUFFER_SIZE,
                                                  &msg_prio,
                                                  &expire);

                        //write to a log file
                        if(num_bytes>0) {
                                fprintf(pfd,"%s  %d  %d  %s\n\n",
                                        ((log_pack*)log)->time_stamp,((log_pack*)log)->log_level,
                                        ((log_pack*)log)->log_source,((log_pack*)log)->log_msg );
                                fflush(pfd);
                        }
                } while(num_bytes>0);
//reregister after emptying the que
                // rc  = mq_notify(msgq,&sig_ev);
// if(rc == -1) {perror("mq_notify-main"); return NULL;}


/*****&&&&&&&&&&&&&&&&&&& printing data for IPC message Q test &&&&&&&&&&&&&&&&&*******/


                clock_gettime(CLOCK_MONOTONIC,&now);
                expire.tv_sec = now.tv_sec+1;
                expire.tv_nsec = now.tv_nsec;
                num_bytes = mq_timedreceive(IPCmsgqT,
                                            (char*)log,
                                            BUFFER_SIZE,
                                            &IPCmsg_prio,
                                            &expire);
                if(num_bytes>0) {printf("data read on IPC msg Q:%s\n",log->time_stamp);}

                clock_gettime(CLOCK_MONOTONIC,&now);
                expire.tv_sec = now.tv_sec+1;
                expire.tv_nsec = now.tv_nsec;
                num_bytes = mq_timedreceive(IPCmsgqL,
                                            (char*)log,
                                            BUFFER_SIZE,
                                            &IPCmsg_prio,
                                            &expire);
                if(num_bytes>0) {printf("data read on IPC msg Q:%s\n",log->time_stamp);}
                sleep(1);
        }
        printf("exiting Log task\n");
        fclose(pfd);
        mq_close(msgq);
        mq_close(messagequeue_error);
        mq_close(IPCmsgqT);
        mq_close(IPCmsgqL);

        free(log);
        return NULL;
}
