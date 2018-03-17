#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include "includes.h"
#include "threads.h"
#include "signals.h"
#include "messageQue.h"
#include <mqueue.h>
#include "errorhandling.h"


void handle_err(char* arg_msg,mqd_t messagequeue_error,mqd_t msgq,msg_type type){
//log on error msg q
        char msg[BUFFER_SIZE];
        sprintf(msg,"%s:%s",arg_msg,strerror(errno));
        err_message_packet err_pack={.type=type};
        strcpy(err_pack.msg, msg);

        struct timespec now,expire;
        int num_bytes;
//log on logegr q
        clock_gettime(CLOCK_MONOTONIC,&now);
        expire.tv_sec = now.tv_sec+2;
        expire.tv_nsec = now.tv_nsec;
        int prio;
        if (type==init) prio = MSG_PRIO_INIT;
        else if (type==error) prio = MSG_PRIO_ERR;

        num_bytes = mq_timedsend(messagequeue_error,
                                 (const char*)&err_pack,
                                 sizeof(err_pack),
                                 prio,
                                 &expire);
        if(num_bytes<0) {perror("mq_send to error Q in handle_err");}

        time_t t = time(NULL); struct tm* tm = localtime(&t);
        log_pack err_log ={.log_level=2,.log_source = error_handler};
        strcpy(err_log.time_stamp, asctime(tm));
        strcpy(err_log.log_msg, "Test Error:");

        clock_gettime(CLOCK_MONOTONIC,&now);
        expire.tv_sec = now.tv_sec+5;
        expire.tv_nsec = now.tv_nsec;

        num_bytes = mq_timedsend(msgq,
                                 (const char*)&err_log,
                                 sizeof(log_pack),
                                 prio,
                                 &expire);
        if(num_bytes<0) {perror("mq_send to log q in handle_err");}


        return;
}


//this thread is created for mq_notify event on error message que
void errorFunction(union sigval sv){
//read and print the error
        if(sv.sival_ptr == NULL) {printf("errorFunction argument\n"); return;}
        mqd_t messagequeue_error = *((mqd_t*)sv.sival_ptr);
        struct timespec now,expire;
        clock_gettime(CLOCK_MONOTONIC,&now);
        expire.tv_sec = now.tv_sec+2;
        expire.tv_nsec = now.tv_nsec;
        int num_bytes;
//empty the msgq - notification only for an empty q
        do {
                num_bytes = mq_timedreceive(messagequeue_error,
                                            (char*)message_packet,
                                            BUFFER_SIZE,
                                            NULL,
                                            &expire);
                if(num_bytes>0) {printf("*%s %d\n",( (err_message_packet*)message_packet)->msg,( (err_message_packet*)message_packet)->type);}

        } while(num_bytes>0);
//reregister for notification
        int rc  = mq_notify(messagequeue_error,&sig_ev_err);
        if(rc == -1) {perror("mq_notify-errorFunction"); return;}
//what are we supposed to do if initializatio complete
//change led status???????????????????????????
//left to do based on err_pack type
        return;
}
