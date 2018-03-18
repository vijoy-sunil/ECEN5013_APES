/***************************************************************
* DATE    : 03/08/2018
* DESCRITPTION  : ERROR HANDLER
                  
* HEADER FILES  : errorhandling.h
****************************************************************/

#include "errorhandling.h"

static pthread_mutex_t error_msg_lock = PTHREAD_MUTEX_INITIALIZER;

void alert(char* msg,mqd_t alertmsg_queue,mqd_t logtask_msg_que,msg_type type){

        alert_message err_pack={.type=type};
        strcpy(err_pack.msg, msg);
        struct timespec current,expire;
        int num_bytes;
        clock_gettime(CLOCK_MONOTONIC,&current);
        expire.tv_sec = current.tv_sec+2;
        expire.tv_nsec = current.tv_nsec;
        int prio;
        if (type==init) prio = INIT_MESSAGE_PRIORITY;
        else if (type==error) prio = ERROR_MESSAGE_PRIORITY;

        if(alertmsg_queue >= 0 ) {
                num_bytes = mq_timedsend(alertmsg_queue,
                                         (const char*)&err_pack,
                                         sizeof(err_pack),
                                         prio,
                                         &expire);
                if(num_bytes<0) {perror("mq_send to alertmsg_queue in notify");}
        }
        if(logtask_msg_que >= 0) {
                time_t t = time(NULL); struct tm* tm = localtime(&t);
                logger_pckt err_log ={.log_level=2,.log_source = error_handler};
                strcpy(err_log.time_stamp, asctime(tm));
                strcpy(err_log.log_msg, msg);

                clock_gettime(CLOCK_MONOTONIC,&current);
                expire.tv_sec = current.tv_sec+5;
                expire.tv_nsec = current.tv_nsec;

                num_bytes = mq_timedsend(logtask_msg_que,
                                         (const char*)&err_log,
                                         sizeof(logger_pckt),
                                         prio,
                                         &expire);
                if(num_bytes<0) {perror("mq_send to logtask_msg_que in notify");}
        }

        return;
}

void alertReceive(union sigval sv){

        if(sv.sival_ptr == NULL) {printf("alertReceive argument\n"); return;}
        mqd_t alertmsg_queue = *((mqd_t*)sv.sival_ptr);
        struct timespec current,expire;
        clock_gettime(CLOCK_MONOTONIC,&current);
        expire.tv_sec = current.tv_sec+2;
        expire.tv_nsec = current.tv_nsec;
        int num_bytes;
        int error_flag = 0;
        int ret =  pthread_mutex_lock(&error_msg_lock);
        if(ret !=0 ) {perror("alertReceive-mutexlock"); return;}

        do {
                bzero(message_packet,sizeof(message_packet));

                num_bytes = mq_timedreceive(alertmsg_queue,
                                            (char*)message_packet,
                                            sizeof(alert_message),
                                            NULL,
                                            &expire);
                if(num_bytes>0) {
                        printf("%s",( (alert_message*)message_packet)->msg);
                        if( ((alert_message*)message_packet)->type == error) error_flag =1;
                }

        } while(num_bytes>0);
        ret =  pthread_mutex_unlock(&error_msg_lock);
        if(ret !=0 ) {perror("alertReceive-mutex unlock"); return;}

        ret  = mq_notify(alertmsg_queue,&signal_event);
        if(ret == -1) {perror("mq_notify-alertReceive"); return;}

#ifdef BBB
        if(error_flag == 1) LED_OPTION(1);
#endif

        return;
}
