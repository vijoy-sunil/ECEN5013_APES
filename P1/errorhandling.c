#include "errorhandling.h"

void ErrorHandler(char* arg_msg,mqd_t messagequeue_error,mqd_t message_queue,msg_type type)
{
        char msg[BUFFER_SIZE];
        sprintf(msg,"%s:%s",arg_msg,strerror(errno));
        err_message_packet err_pack={.type=type};
        strcpy(err_pack.msg, msg);

        struct timespec current,timeout;
        int nbytes;

        clock_gettime(CLOCK_MONOTONIC,&current);
        timeout.tv_sec = current.tv_sec+2;
        timeout.tv_nsec = current.tv_nsec;
        int prio;
        if (type==init) 
            prio = MESSAGE_PRIORITY_INIT;
        else if 
            (type==error) prio = MESSAGE_PRIORITY_ERR;

        nbytes = mq_timedsend(messagequeue_error,
                                 (const char*)&err_pack,
                                 sizeof(err_pack),
                                 prio,
                                 &timeout);
        if(nbytes<0) {perror("mq_send to error Q in ErrorHandler");}

        time_t t = time(NULL); struct tm* tm = localtime(&t);
        log_pack err_log ={.log_level=2,.log_source = ErrorTask};
        strcpy(err_log.time_stamp, asctime(tm));
        strcpy(err_log.log_msg, "Test Error:");

        clock_gettime(CLOCK_MONOTONIC,&current);
        timeout.tv_sec = current.tv_sec+5;
        timeout.tv_nsec = current.tv_nsec;

        nbytes = mq_timedsend(message_queue,
                                 (const char*)&err_log,
                                 sizeof(log_pack),
                                 prio,
                                 &timeout);
        if(nbytes<0) {perror("mq_send to log q in ErrorHandler");}


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
        struct timespec current,timeout;
        clock_gettime(CLOCK_MONOTONIC,&current);
        timeout.tv_sec = current.tv_sec+2;
        timeout.tv_nsec = current.tv_nsec;
        int nbytes;

        do {
                nbytes = mq_timedreceive(messagequeue_error,
                                            (char*)message_packet,
                                            BUFFER_SIZE,
                                            NULL,
                                            &timeout);
                if(nbytes>0) 
                {
                    printf("*%s %d\n",( (err_message_packet*)message_packet)->msg,( (err_message_packet*)message_packet)->type);
                }

        } while(nbytes>0);

    int rc  = mq_notify(messagequeue_error,&event_error);
    if(rc == -1) 
    {
        perror("mq_notify-ErrorNotify"); 
        return;
    }
        return;
}
