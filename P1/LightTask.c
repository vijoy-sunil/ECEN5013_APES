#include "LightTask.h"

sig_atomic_t Light_Msg_flag;


void *LightTask(void *pthread_inf) {

        uint8_t initialize = 1;
        char initialize_msg[8][ sizeof(alert_message) ];

        Light_Msg_flag = 0;
        int ret;
        threadTaskAttr *pthread_info = (threadTaskAttr *)pthread_inf;

        mqd_t alertmsg_queue;
        int msg_prio_err = ERROR_MESSAGE_PRIORITY;
        int error_no;
        struct mq_attr msgq_attr_err = {.mq_maxmsg = MESSAGEQ_SIZE, .mq_msgsize = BUFFER_SIZE,.mq_flags = 0};

        alertmsg_queue = mq_open(ALERT_MSGQ_PCKT, O_CREAT | O_RDWR, S_IRWXU, &msgq_attr_err); 

        if(alertmsg_queue < 0) {
                initialize =0;
                sprintf(&(initialize_msg[0][0]),"FAILED light msgq %s\n",strerror(errno));
        }
        else {
                sprintf(&(initialize_msg[0][0]),"SUCCESS light msgq:%s\n",strerror(errno));
        }


        ret = timer_init_light();
        if(ret == -1) {
                initialize =0;
                sprintf(&(initialize_msg[7][0]),"Failure timer_init_light\n");
        }
        else {
                sprintf(&(initialize_msg[7][0]),"Success timer_init_light\n");

        }

        ret = pthread_mutex_init(&glight_mutex,NULL);
        if(ret == -1) {
                initialize =0;
                sprintf(&(initialize_msg[1][0]),"Failure glight_mutex %s\n",strerror(errno));
        }
        else {
                sprintf(&(initialize_msg[1][0]),"Success glight_mutex%s\n",strerror(errno));
        }

        mqd_t logtask_msg_que;
        int msg_prio = MESSAGE_PRIORITY;
        int num_bytes;
        char message[BUFFER_SIZE];
        struct mq_attr msgq_attr = {.mq_maxmsg = MESSAGEQ_SIZE, .mq_msgsize = BUFFER_SIZE,.mq_flags = 0};

        logtask_msg_que = mq_open(LOGGER_MSGQ_IPC, O_CREAT | O_RDWR,S_IRWXU,&msgq_attr); 

        if(logtask_msg_que < 0) {
                initialize =0;
                sprintf(&(initialize_msg[2][0]),"Failure log msq open %s\n",strerror(errno));
        }
        else {
                sprintf(&(initialize_msg[2][0]),"Success log msq open %s\n",strerror(errno));
        }

        mqd_t messageq_ipc;
        int IPCmsg_prio = 20;
        int IPCnum_bytes;

        struct mq_attr IPCmsgq_attr = {.mq_maxmsg = MESSAGEQ_SIZE,.mq_msgsize = BUFFER_SIZE,.mq_flags = 0};

        messageq_ipc = mq_open(LIGHT_MSGQ_IPC,O_CREAT | O_RDWR,S_IRWXU,&IPCmsgq_attr); 

        if(messageq_ipc < 0) {
                initialize =0;
                sprintf(&(initialize_msg[3][0]),"Failure msgq ipc %s\n",strerror(errno));
        }
        else {
                sprintf(&(initialize_msg[3][0]),"Success msgq ipc %s\n",strerror(errno));
        }

        struct sigaction sigactn;
        sigemptyset(&sigactn.sa_mask);
        sigactn.sa_handler = Handl_LightMsg;
        ret = sigaction(LIGHTSIGNAL_PACKET,&sigactn,NULL);
        if(ret  == -1) {
                initialize =0;
                sprintf(&(initialize_msg[4][0]),"LightTask sigaction %s\n",strerror(errno));
        }
        else {
                sprintf(&(initialize_msg[4][0]),"LightTask sigaction %s\n",strerror(errno));
        }
#ifdef BBB

        int light = init_light_sensor();
        if(light  == -1) {
                initialize =0;
                sprintf(&(initialize_msg[5][0]),"Light sensor init Failed \n");
        }
        else {
                sprintf(&(initialize_msg[5][0]),"Light sensor init Success \n");
        }
        char lightbuffer[1];
        commandReg(light,CONTROL,WRITE);
        controlReg(light,WRITE,ENABLE,lightbuffer);

        float lumen;
        char data_lumen_str[BUFFER_SIZE-200];
        uint16_t ch0,ch1;
        status day_night = DAY;

#endif

        sigset_t mask_bit; 
        sigemptyset(&mask_bit);
        sigaddset(&mask_bit,TEMPERATURE_SIGNAL_OPT); sigaddset(&mask_bit,TEMPERATURE_SIG_HEARTBEAT);
        sigaddset(&mask_bit,LOGGER_SIG); sigaddset(&mask_bit,LIGHT_SIG_HEARTBEAT);
        sigaddset(&mask_bit,LOGGER_SIG_HEARTBEAT); sigaddset(&mask_bit,SIGCONT);
        sigaddset(&mask_bit,SOCKET_SIG_HEARTBEAT);

//unblocking for test
//sigaddset(&mask_bit,TEMPSIGNAL_PACKET); sigaddset(&mask_bit,LIGHTSIGNAL_PACKET);

        ret = pthread_sigmask(
                SIG_SETMASK, //block the signals in the set argument
                &mask_bit, //set argument has list of blocked signals
                NULL); //if non NULL prev val of signal mask_bit stored here
        if(ret  == -1) {
                initialize =0;
                sprintf(&(initialize_msg[6][0]),"LightTask pthread_sigmask %s\n",strerror(errno));
        }
        else {
                sprintf(&(initialize_msg[6][0]),"LightTask pthread_sigmask %s\n",strerror(errno));
        }
//send initialization status

        notify(&initialize_msg[0][0],alertmsg_queue,logtask_msg_que,init);
        notify(&initialize_msg[1][0],alertmsg_queue,logtask_msg_que,init);
        notify(&initialize_msg[2][0],alertmsg_queue,logtask_msg_que,init);
        notify(&initialize_msg[3][0],alertmsg_queue,logtask_msg_que,init);
        notify(&initialize_msg[4][0],alertmsg_queue,logtask_msg_que,init);
        notify(&initialize_msg[5][0],alertmsg_queue,logtask_msg_que,init);
        notify(&initialize_msg[6][0],alertmsg_queue,logtask_msg_que,init);
        notify(&initialize_msg[7][0],alertmsg_queue,logtask_msg_que,init);

        if(initialize == 0) { notify("##All elements not initialized in Light Task, Not proceeding with it##\n",alertmsg_queue,logtask_msg_que,error);
                              while(light_close_flag & application_close_flag) {sleep(1);};
                              return NULL;}

        else if(initialize == 1) notify("##All elements initialized in Light Task, proceeding with it##\n",alertmsg_queue,logtask_msg_que,init);


        //  sleep(2);//allow other threads to Initialize

/************Creating logpacket*******************/
        log_pack light_log ={.log_level=1,.log_source = light_Task};
        struct timespec now,expire;


/****************Do this periodically*******************************/
        while(light_close_flag & application_close_flag) {

                pthread_kill(pthread_info->main,LIGHT_SIG_HEARTBEAT);//send HB

                pthread_mutex_lock(&glight_mutex);
                while(glight_flag == 0) {
                        pthread_cond_wait(&glight_condition,&glight_mutex);
                }
                pthread_mutex_unlock(&glight_mutex);
                glight_flag = 0;
#ifdef BBB
/*************collect data*****************/
                ch0=adcDataRead(light,0);
                ch1=adcDataRead(light,1);
                lumen = reportLumen(ch0, ch1);

/************populate the log packet*********/
                sprintf(data_lumen_str,"lumen %f", lumen);
                strcpy(light_log.log_msg, data_lumen_str);

#else
                strcpy(light_log.log_msg, "Mock lumen");
#endif

                time_t t = time(NULL); struct tm *tm = localtime(&t);
                strcpy(light_log.time_stamp, asctime(tm));

/************Log messages on Que*************/
                clock_gettime(CLOCK_MONOTONIC,&now);
                expire.tv_sec = now.tv_sec+2;
                expire.tv_nsec = now.tv_nsec;
                num_bytes = mq_timedsend(logtask_msg_que,
                                         (const char*)&light_log,
                                         sizeof(log_pack),
                                         msg_prio,
                                         &expire);
                if(num_bytes<0) {notify("mq_send-Log Q-LightTask",
                                        alertmsg_queue,logtask_msg_que,error);}

#ifdef BBB
                if(day_night != reportStatus(light)) {
                        day_night = reportStatus(light);
                        sprintf(data_lumen_str,"Day Night State change\n");
                        strcpy(light_log.log_msg, data_lumen_str);

                        num_bytes = mq_timedsend(logtask_msg_que,
                                                 (const char*)&light_log,
                                                 sizeof(log_pack),
                                                 msg_prio,
                                                 &expire);
                        if(num_bytes<0) {notify("mq_send-Log Q-LightTask",
                                                alertmsg_queue,logtask_msg_que,error);}

                }

#endif

/******Log data on IPC Que if requested******/

                if(Light_Msg_flag == 1) {
                        Light_Msg_flag = 0;
//set up time for timed send

                        clock_gettime(CLOCK_MONOTONIC,&now);
                        expire.tv_sec = now.tv_sec+2;
                        expire.tv_nsec = now.tv_nsec;
                        num_bytes = mq_timedsend(messageq_ipc,
                                                 (const char*)&light_log,
                                                 sizeof(log_pack),
                                                 IPCmsg_prio,
                                                 &expire);
                        if(num_bytes<0) {notify("mq_send-IPC-LightTask Error",alertmsg_queue,logtask_msg_que,error);}
                }


        }
        printf("exiting light task\n");
        mq_close(logtask_msg_que);
        mq_close(alertmsg_queue);
        mq_close(messageq_ipc);
        return NULL;

}


void Handl_LightMsg(int sig){
        if(sig == LIGHTSIGNAL_PACKET)
        {printf("Caught signal Handl_LightMsg\n");
         Light_Msg_flag = 1;}
}