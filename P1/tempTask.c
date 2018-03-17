#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include "tasks.h"
#include "notify.h"
#include "messageQue.h"
#include <mqueue.h>
#include "packetdefinition.h"
#include "errorhandling.h"

#include "tmp102.h"


sig_atomic_t temp_IPC_flag;

void TemptIPChandler(int sig){
        if(sig == TEMPERATURE_SIGNAL_IPC)
        {printf("Caught signal TemptIPChandler\n");
         temp_IPC_flag = 1;}
}

void *tempTask(void *pthread_inf) {

        uint8_t init_state = 0;
        char init_message[6][ sizeof(err_message_packet) ];


        temp_IPC_flag = 0;
        int rc;
        threadPckt *ppthread_info = (threadPckt *)pthread_inf;



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
        // if(messagequeue_error < 0) {perror("mq_open-error_mq-tempTask "); return NULL;}
// else printf("Messgae Que Opened in tempTask\n");

        if(messagequeue_error < 0) {
                init_state =0;
                sprintf(&(init_message[0][0]),"mq_open-error_mq-tempTask %s\n",strerror(errno));
        }
        else {
                init_state =0;
                sprintf(&(init_message[0][0]),"MessgaeQ Opened in tempTask:%s\n",strerror(errno));
        }



/******set periodic timer**************/
        rc = ConfigTimerTemp(); //sets up timer to periodically signal and wake this thread
        if(rc ==-1) return NULL;
        else printf("Periodic Timer set for Temperature Task\n");

        rc = pthread_mutex_init(&temperature_mutex,NULL);
        if(rc == -1) {
                init_state =0;
                sprintf(&(init_message[1][0]),"temptask-pthread_mutex_init %s\n",strerror(errno));
        }
        else {
                init_state =0;
                sprintf(&(init_message[1][0]),"temptaskpthread_mutex_init:%s\n",strerror(errno));
        }


/*******Initialize Logger Message Que*****************/
        mqd_t msgq;
        int msg_prio = 30;
        int num_bytes;
        char message[BUFFER_SIZE];
        struct mq_attr msgq_attr = {.mq_maxmsg = MAX_QUE_MSGSIZE, //max # msg in queue
                                    .mq_msgsize = BUFFER_SIZE,//max size of msg in bytes
                                    .mq_flags = 0};

        msgq = mq_open(MY_MQ, //name
                       O_CREAT | O_RDWR,//flags. create a new if dosent already exist
                       S_IRWXU, //mode-read,write and execute permission
                       &msgq_attr); //attribute

        if(msgq < 0) {
                init_state =0;
                sprintf(&(init_message[2][0]),"temptask-mq_open-loggerQ %s\n",strerror(errno));
        }
        else {
                init_state =0;
                sprintf(&(init_message[2][0]),"temptask-mq_open-loggerQ %s\n",strerror(errno));
        }




/***************setting msgq for IPC data Request******************/
        mqd_t IPCmsgq;
        int IPCmsg_prio = 20;
        int IPCnum_bytes;
        char IPCmessage[BUFFER_SIZE];

        struct mq_attr IPCmsgq_attr = {.mq_maxmsg = MAX_QUE_MSGSIZE, //max # msg in queue
                                       .mq_msgsize = BUFFER_SIZE, //max size of msg in bytes
                                       .mq_flags = 0};

        IPCmsgq = mq_open(IPC_TEMP_MQ, //name
                          O_CREAT | O_RDWR, //flags. create a new if dosent already exist
                          S_IRWXU, //mode-read,write and execute permission
                          &IPCmsgq_attr); //attribute
        if(IPCmsgq < 0) {
                init_state =0;
                sprintf(&(init_message[3][0]),"temptask-mq_open-IPCQ %s\n",strerror(errno));
        }
        else {
                init_state =1;
                sprintf(&(init_message[3][0]),"temptask-mq_open-IPCQ %s\n",strerror(errno));
        }
//set up the signal to request data
        struct sigaction action;
        sigemptyset(&action.sa_mask);
        action.sa_handler = TemptIPChandler;
        rc = sigaction(TEMPERATURE_SIGNAL_IPC,&action,NULL);

        if(rc  == -1) {
                init_state =0;
                sprintf(&(init_message[4][0]),"sigaction temptask %s\n",strerror(errno));
        }
        else {
                init_state =1;
                sprintf(&(init_message[4][0]),"sigaction temptask %s\n",strerror(errno));
        }

        int temp = initialize_temp();
        char temp_data[2], data_cel_str[8];
        float data_cel;

        if(temp  == -1) {
                init_state =0;
                sprintf(&(init_message[5][0]),"i2c init fail\n");
        }
        else {
                init_state =1;
                sprintf(&(init_message[5][0]),"i2c init success\n");
        }

        sleep(1);//let other threads initialize

        handle_err(&init_message[0][0],messagequeue_error,msgq,init);
        handle_err(&init_message[1][0],messagequeue_error,msgq,init);
        handle_err(&init_message[2][0],messagequeue_error,msgq,init);
        handle_err(&init_message[3][0],messagequeue_error,msgq,init);
        handle_err(&init_message[4][0],messagequeue_error,msgq,init);
        handle_err(&init_message[5][0],messagequeue_error,msgq,init);

        struct timespec now,expire;

        char buffer[3];

        tlow_read(temp,buffer);
        printf("TEMP SENSOR TLOW READ %x %x \n",buffer[0],buffer[1] );

        thigh_write(temp,buffer);
        buffer[0]=buffer[1]=0;
        thigh_read(temp,buffer);
        printf("TEMP SENSOR THIGH READ %x %x \n",buffer[0],buffer[1] );

        buffer[0]=TEMP_CONFIG_REG;
        buffer[1]=SHUTDOWN_DI;
        configreg_write(temp,buffer);

        printf("TEMP SENSOR BEFORE SHUTDOWN %x %x \n",buffer[0],buffer[1] );

        buffer[0]=TEMP_CONFIG_REG;
        buffer[1]=SHUTDOWN_EN;

        configreg_write(temp,buffer);

        buffer[0]=buffer[1]=0;
        configreg_read(temp,buffer);

        printf("TEMP SENSOR AFTER SHUTDOWN %x %x \n",buffer[0],buffer[1] );

        buffer[0]=TEMP_CONFIG_REG;
        buffer[1]=SHUTDOWN_DI;
        configreg_write(temp,buffer);

        buffer[0]=TEMP_CONFIG_REG;
        buffer[1]=RES_10BIT;
        buffer[2]= EMMODE|CONVRATE3;

        configreg_write(temp,buffer);

        buffer[0]=buffer[1]=0;
        configreg_read(temp,buffer);

        printf("TEMPSENSOR 10 BIT RESOLUTION, EMMODE AND CONV RATE 8Hz- %x %x \n",buffer[0],buffer[1] );

        printf("FAULT BITS ARE %d %d\n",(buffer[0]&0x08)>>3,(buffer[0]&0x10)>>4);

/************Creating logpacket*******************/
        log_pack temp_log ={.log_level=1,.log_source = temperatue_Task};

/****************Do this periodically*******************************/
        while(temp_close & app_close) {

                pthread_kill(ppthread_info->main_task,TEMPERATURE_HB_SIG);//send HB
                pthread_mutex_lock(&temperature_mutex);
                while(temperature_flag == 0) {
                        pthread_cond_wait(&temperature_cond_var,&temperature_mutex);
                }

                pthread_mutex_unlock(&temperature_mutex);
                temperature_flag = 0;
//collect temperatue
                temperature_read(temp,temp_data); 
                data_cel = temp_conv(CELCIUS,temp_data);

/************populate the log packet*********/

                time_t t = time(NULL); struct tm *tm = localtime(&t);
                strcpy(temp_log.time_stamp, asctime(tm));
                sprintf(data_cel_str,"temperature %f", data_cel);
                strcpy(temp_log.log_msg, data_cel_str);

/*******Log messages on Que*************/
//set up time for timed send

                clock_gettime(CLOCK_MONOTONIC,&now);
                expire.tv_sec = now.tv_sec+2;
                expire.tv_nsec = now.tv_nsec;
                num_bytes = mq_timedsend(msgq,
                                         (const char*)&temp_log,
                                         sizeof(log_pack),
                                         msg_prio,
                                         &expire);
                if(num_bytes<0) {handle_err("mq_send to Log Q in tempTask", messagequeue_error,msgq,error);}
/******Log data on IPC Que if requested******/

                if(temp_IPC_flag == 1) {
                        temp_IPC_flag = 0;
//set up time for timed send
                        clock_gettime(CLOCK_MONOTONIC,&now);
                        expire.tv_sec = now.tv_sec+2;
                        expire.tv_nsec = now.tv_nsec;
                        num_bytes = mq_timedsend(IPCmsgq,
                                                 (const char*)&temp_log,
                                                 sizeof(log_pack),
                                                 IPCmsg_prio,
                                                 &expire);
                        if(num_bytes<0) {handle_err("mq_send-IPC Q-tempTask Error",messagequeue_error,msgq,error);}
                        else printf("data put on IPC msg Q\n");
                }
                //printf("hi\n");
                //handle_err("Test Error",messagequeue_error,msgq,error);


        }
        printf("exiting Temp task\n");
        mq_close(msgq);
        mq_close(messagequeue_error);
        mq_close(IPCmsgq);

        return NULL;
}
