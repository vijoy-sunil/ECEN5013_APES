#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "tasks.h"
#include "queue.h"
#include <mqueue.h>
#include "packetdefinition.h"
#include "notify.h"
#include "errorhandling.h"

// void LogQNotifyhandler(int sig){
//         if(sig == LOGGER_SIGNAL)
//                 printf("received data\n");
//
// }

sig_atomic_t log_data_flag;

void *logTask(void *pthread_pckt) {
        int rc;
//log_data_flag=0;
        threadPckt *ppthread_pckto = (threadPckt *)pthread_pckt;
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
        int message_priority_err = MESSAGE_PRIORITY_ERR;
        int error_size;
        struct mq_attr messagequeue_attr_error = {.mq_maxmsg = MAX_QUE_MSGSIZE, //max # msg in queue
                                        .mq_msgsize = BUFFER_SIZE,//max size of msg in bytes
                                        .mq_flags = 0};

        messagequeue_error = mq_open(MESSAGE_Q_ERR, //name
                           O_CREAT | O_RDWR,//flags. create a new if dosent already exist
                           S_IRWXU, //mode-read,write and execute permission
                           &messagequeue_attr_error); //attribute
        if(messagequeue_error < 0) {perror("mq_open-error_mq-TemperatureTask "); return NULL;}
        else printf("Messgae Que Opened in TemperatureTask\n");



/*************create a logger message q****************/
        mqd_t message_queue;
        int message_priority;
        int nbytes;
        //char message[BUFFER_SIZE];
        log_pack* log = (log_pack*)malloc(sizeof(log_pack));
        if(log == NULL) {perror("malloc-logTask Err"); return NULL;}
        struct mq_attr message_queue_attr = {.mq_maxmsg = MAX_QUE_MSGSIZE, //max # msg in queue
                                    .mq_msgsize = BUFFER_SIZE,//max size of msg in bytes
                                    .mq_flags = 0};

        message_queue = mq_open(MESSAGE_Q, //name
                       O_CREAT | O_RDWR,//flags. create a new if dosent already exist
                       S_IRWXU, //mode-read,write and execute permission
                       &message_queue_attr); //attribute
        if(message_queue < 0) {perror("mq_open-logTask Error:"); return NULL;}
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
//         rc  = mq_notify(message_queue,&sig_ev);
//         if(rc == -1) {perror("mq_notify-main"); return NULL;}



//open a file to write to
        FILE* pfd = fopen("logfile.txt","w");
        if(pfd==NULL) {perror("fopen-logTask Error:"); return NULL;}
        else printf("log file opened in logTask\n");


/*******&&&&&&&&&&&&&& msg Q to test IPC transfer&&&&&&&&&&&&&&&&&&**********************/
        mqd_t IPCmessage_queueT, IPCmessage_queueL;
        int IPCmessage_priority = 20;
        int IPCnbytes;
        char IPCmessage[BUFFER_SIZE];

        struct mq_attr IPCmessage_queue_attr = {.mq_maxmsg = MAX_QUE_MSGSIZE, //max # msg in queue
                                       .mq_msgsize = BUFFER_SIZE, //max size of msg in bytes
                                       .mq_flags = 0};
//temp
        IPCmessage_queueT = mq_open(TEMPERATURE_MESSAGE_Q, //name
                           O_CREAT | O_RDWR, //flags. create a new if dosent already exist
                           S_IRWXU, //mode-read,write and execute permission
                           &IPCmessage_queue_attr); //attribute
        if(IPCmessage_queueT < 0) {perror("mq_open-logTask Error:"); return NULL;}
        else printf("IPC temp Messgae Que Opened in logTask\n");
//light
        IPCmessage_queueL = mq_open(LIGHT_MESSAGE_Q, //name
                           O_CREAT | O_RDWR, //flags. create a new if dosent already exist
                           S_IRWXU, //mode-read,write and execute permission
                           &IPCmessage_queue_attr); //attribute
        if(IPCmessage_queueL < 0) {perror("mq_open-logTask Error:"); return NULL;}
        else printf("IPC light Messgae Que Opened in logTask\n");
        struct timespec current,timeout;

/*******************Do this in LOOP************************************/
        while(logger_close & app_close)
        {

                pthread_kill(ppthread_pckto->main_task,LOGGER_HB_SIG);//send HB
//empty the message que
                do {
                        //read from queue
                        clock_gettime(CLOCK_MONOTONIC,&current);
                        timeout.tv_sec = current.tv_sec+5;
                        timeout.tv_nsec = current.tv_nsec;

                        nbytes=mq_timedreceive(message_queue,
                                                  (char*)log,
                                                  BUFFER_SIZE,
                                                  &message_priority,
                                                  &timeout);

                        //write to a log file
                        if(nbytes>0) {
                                fprintf(pfd,"%s  %d  %d  %s\n\n",
                                        ((log_pack*)log)->time_stamp,((log_pack*)log)->log_level,
                                        ((log_pack*)log)->log_source,((log_pack*)log)->log_msg );
                                fflush(pfd);
                        }
                } while(nbytes>0);
//reregister after emptying the que
                // rc  = mq_notify(message_queue,&sig_ev);
// if(rc == -1) {perror("mq_notify-main"); return NULL;}


/*****&&&&&&&&&&&&&&&&&&& printing data for IPC message Q test &&&&&&&&&&&&&&&&&*******/


                clock_gettime(CLOCK_MONOTONIC,&current);
                timeout.tv_sec = current.tv_sec+1;
                timeout.tv_nsec = current.tv_nsec;
                nbytes = mq_timedreceive(IPCmessage_queueT,
                                            (char*)log,
                                            BUFFER_SIZE,
                                            &IPCmessage_priority,
                                            &timeout);
                if(nbytes>0) {printf("data read on IPC msg Q:%s\n",log->time_stamp);}

                clock_gettime(CLOCK_MONOTONIC,&current);
                timeout.tv_sec = current.tv_sec+1;
                timeout.tv_nsec = current.tv_nsec;
                nbytes = mq_timedreceive(IPCmessage_queueL,
                                            (char*)log,
                                            BUFFER_SIZE,
                                            &IPCmessage_priority,
                                            &timeout);
                if(nbytes>0) {printf("data read on IPC msg Q:%s\n",log->time_stamp);}
                sleep(1);
        }
        printf("exiting Log task\n");
        fclose(pfd);
        mq_close(message_queue);
        mq_close(messagequeue_error);
        mq_close(IPCmessage_queueT);
        mq_close(IPCmessage_queueL);

        free(log);
        return NULL;
}
