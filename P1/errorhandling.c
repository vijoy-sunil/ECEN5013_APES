#include "errorhandling.h"

static pthread_mutex_t error_msg_lock = PTHREAD_MUTEX_INITIALIZER;

/****************puts initialization messages and errors on notify msg q and logger msg q. notify msg q is registered to notify main, which creates a thread to handle these conditions appropriately*************/
void notify(char* msg,mqd_t alertmsg_queue,mqd_t logger_msgq,msg_type type){
        //char msg[BUFFER_SIZE];
        //sprintf(msg,"%s:%s",arg_msg,strerror(errno));

        alert_message err_pack={.type=type};
        strcpy(err_pack.msg, msg);
        struct timespec now,expire;
        int num_bytes;
        clock_gettime(CLOCK_MONOTONIC,&now);
        expire.tv_sec = now.tv_sec+2;
        expire.tv_nsec = now.tv_nsec;
        int prio;
        if (type==init) prio = INIT_MESSAGE_PRIORITY;
        else if (type==error) prio = ERROR_MESSAGE_PRIORITY;

//log on notify msg q
        if(alertmsg_queue >= 0 ) {
                num_bytes = mq_timedsend(alertmsg_queue,
                                         (const char*)&err_pack,
                                         sizeof(err_pack),
                                         prio,
                                         &expire);
                if(num_bytes<0) {perror("mq_send to alertmsg_queue in notify");}
        }
        if(logger_msgq >= 0) {
                time_t t = time(NULL); struct tm* tm = localtime(&t);
                log_pack err_log ={.log_level=2,.log_source = error_handler};
                strcpy(err_log.time_stamp, asctime(tm));
                strcpy(err_log.log_msg, msg);

//log on logegr q
                clock_gettime(CLOCK_MONOTONIC,&now);
                expire.tv_sec = now.tv_sec+5;
                expire.tv_nsec = now.tv_nsec;

                num_bytes = mq_timedsend(logger_msgq,
                                         (const char*)&err_log,
                                         sizeof(log_pack),
                                         prio,
                                         &expire);
                if(num_bytes<0) {perror("mq_send to logger_msgq in notify");}
        }

        return;
}

/*This thread is invoked via mq_notify on main once any initialization or error message is put on alertmsg_queue
 */
void alertReceive(union sigval sv){
//read and print the error
        if(sv.sival_ptr == NULL) {printf("alertReceive argument\n"); return;}
        mqd_t alertmsg_queue = *((mqd_t*)sv.sival_ptr);
        struct timespec now,expire;
        clock_gettime(CLOCK_MONOTONIC,&now);
        expire.tv_sec = now.tv_sec+2;
        expire.tv_nsec = now.tv_nsec;
        int num_bytes;
        int error_flag = 0;
        int ret =  pthread_mutex_lock(&error_msg_lock);
        if(ret !=0 ) {perror("alertReceive-mutexlock"); return;}
//empty the logger_msgq - notification only for an empty q
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

//reregister for notification
        ret  = mq_notify(alertmsg_queue,&signal_event);
        if(ret == -1) {perror("mq_notify-alertReceive"); return;}

//For error condition turn on the LED
#ifdef BBB
        if(error_flag == 1) LED_OPTION(1);
#endif

        return;
}
