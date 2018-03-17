#include "errorhandling.h"

void ErrorHandler(char* arg_msg,mqd_t messagequeue_error,mqd_t msgq,msg_type type)
{
        char msg[BUFFER_SIZE];
        sprintf(msg,"%s:%s",arg_msg,strerror(errno));
        err_message_packet err_pack={.type=type};
        strcpy(err_pack.msg, msg);

        struct timespec now,expire;
        int num_bytes;

        clock_gettime(CLOCK_MONOTONIC,&now);
        expire.tv_sec = now.tv_sec+2;
        expire.tv_nsec = now.tv_nsec;
        int prio;
        if (type==init) 
            prio = MESSAGE_PRIORITY_INIT;
        else if 
            (type==error) prio = MESSAGE_PRIORITY_ERR;

        num_bytes = mq_timedsend(messagequeue_error,
                                 (const char*)&err_pack,
                                 sizeof(err_pack),
                                 prio,
                                 &expire);
        if(num_bytes<0) {perror("mq_send to error Q in ErrorHandler");}

        time_t t = time(NULL); struct tm* tm = localtime(&t);
        log_pack err_log ={.log_level=2,.log_source = ErrorTask};
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
        if(num_bytes<0) {perror("mq_send to log q in ErrorHandler");}


        return;
}


void ErrorNotify(union sigval sv)
{
        if(sv.sival_ptr == NULL) 
        {
            printf("ErrorNotify argument\n"); 
            return;
        }

        mqd_t messagequeue_error = *((mqd_t*)sv.sival_ptr);
        struct timespec now,expire;
        clock_gettime(CLOCK_MONOTONIC,&now);
        expire.tv_sec = now.tv_sec+2;
        expire.tv_nsec = now.tv_nsec;
        int num_bytes;

        do {
                num_bytes = mq_timedreceive(messagequeue_error,
                                            (char*)message_packet,
                                            BUFFER_SIZE,
                                            NULL,
                                            &expire);
                if(num_bytes>0) 
                {
                    printf("*%s %d\n",( (err_message_packet*)message_packet)->msg,( (err_message_packet*)message_packet)->type);
                }

        } while(num_bytes>0);

    int rc  = mq_notify(messagequeue_error,&event_error);
    if(rc == -1) 
    {
        perror("mq_notify-ErrorNotify"); 
        return;
    }
        return;
}
