#include "TemperatureTask.h"

static sig_atomic_t IPC_Flag;

static void IPC_Handler(int sig){
    if(sig == TEMPERATURE_SIGNAL_IPC)
    {
         IPC_Flag = 1;
    }
}


// void pwrdn_sighandler(int signo)
// {
//     printf("\n sighandler called \n");
//     if (signo==SIGIO) {
//         printf("\t**************** GPIO interrupt raised! *********************\n");
//         // led here
//     }
//     return;
// }

void *TemperatureTask(void *pthread_pckt) {

        uint8_t initialize = 0;
        char initialize_message[6][ sizeof(err_message_packet) ];


        IPC_Flag = 0;
        int rc;
        threadPckt *ppthread_pckto = (threadPckt *)pthread_pckt;

        mqd_t messagequeue_error;
        int message_priority_err = MESSAGE_PRIORITY_ERR;
        int error_size;
        struct mq_attr messagequeue_attr_error = {.mq_maxmsg = MAX_QUE_MSGSIZE,.mq_msgsize = BUFFER_SIZE,.mq_flags = 0};

        messagequeue_error = mq_open(MESSAGE_Q_ERR,O_CREAT | O_RDWR,S_IRWXU,&messagequeue_attr_error);

        if(messagequeue_error < 0) {
                initialize =0;
                sprintf(&(initialize_message[0][0]),"Failed to open Temp Msg q %s\n",strerror(errno));
        }
        else {
                initialize =0;
                sprintf(&(initialize_message[0][0])," Temp Msg q Opened:%s\n",strerror(errno));
        }

        rc = ConfigTimerTemp();

        if(rc ==-1) 
            return NULL;
        else 
            printf("Timer Configured\n");

        rc = pthread_mutex_init(&temperature_mutex,NULL);
        if(rc == -1) {
                initialize =0;
                sprintf(&(initialize_message[1][0]),"Temp task mutex failed %s\n",strerror(errno));
        }
        else
        {
                initialize =0;
        }

        mqd_t message_queue;
        int message_priority = 30;
        int nbytes;
        char message[BUFFER_SIZE];
        struct mq_attr message_queue_attr = {.mq_maxmsg = MAX_QUE_MSGSIZE,.mq_msgsize = BUFFER_SIZE,.mq_flags = 0};

        message_queue = mq_open(MESSAGE_Q,O_CREAT | O_RDWR,S_IRWXU,  &message_queue_attr);

        if(message_queue < 0) {
                initialize =0;
                sprintf(&(initialize_message[2][0]),"Log msgq failed %s\n",strerror(errno));
        }
        else
        {
                initialize =0;
        }

        mqd_t IPCmessage_queue;
        int IPCmessage_priority = 20;
        int IPCnbytes;
        char IPCmessage[BUFFER_SIZE];

        struct mq_attr IPCmessage_queue_attr = {.mq_maxmsg = MAX_QUE_MSGSIZE, .mq_msgsize = BUFFER_SIZE,.mq_flags = 0};

        IPCmessage_queue = mq_open(TEMPERATURE_MESSAGE_Q,O_CREAT | O_RDWR,S_IRWXU, &IPCmessage_queue_attr); 

        if(IPCmessage_queue < 0) {
                initialize =0;
                sprintf(&(initialize_message[3][0]),"msq failed %s\n",strerror(errno));
        }
        else {
                initialize =1;
                sprintf(&(initialize_message[3][0]),"msq success %s\n",strerror(errno));
        }

        struct sigaction action;
        sigemptyset(&action.sa_mask);
        action.sa_handler = IPC_Handler;
        rc = sigaction(TEMPERATURE_SIGNAL_IPC,&action,NULL);

        if(rc  == -1) {
                initialize =0;
                sprintf(&(initialize_message[4][0]),"sigaction failed %s\n",strerror(errno));
        }
        else {
                initialize =1;
                sprintf(&(initialize_message[4][0]),"sigaction success %s\n",strerror(errno));
        }

        int temp = initialize_temp();
        char temp_data[2], data_cel_str[8];
        float data_cel;

        if(temp  == -1) {
                initialize =0;
                sprintf(&(initialize_message[5][0]),"i2c init fail\n");
        }
        else {
                initialize =1;
                sprintf(&(initialize_message[5][0]),"i2c init success\n");
        }

        sleep(1);//let other threads initialize

        ErrorHandler(&initialize_message[0][0],messagequeue_error,message_queue,init);
        ErrorHandler(&initialize_message[1][0],messagequeue_error,message_queue,init);
        ErrorHandler(&initialize_message[2][0],messagequeue_error,message_queue,init);
        ErrorHandler(&initialize_message[3][0],messagequeue_error,message_queue,init);
        ErrorHandler(&initialize_message[4][0],messagequeue_error,message_queue,init);
        ErrorHandler(&initialize_message[5][0],messagequeue_error,message_queue,init);

        struct timespec current,timeout;

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
        //interrupt
        // int pwrdn_fd;
        // int count;
        // struct sigaction temp_int_action;
        // char input;
        // memset(&action, 0, sizeof(temp_int_action));
        // action.sa_handler = pwrdn_sighandler;
        // action.sa_flags = 0;

        // sigaction(SIGIO, &temp_int_action, NULL);

        // pwrdn_fd = open("/dev/gpio_int", O_RDWR);

        // if (pwrdn_fd < 0) {
        // perror("Failed to open device\n");
        // //return 1;
        // }

        // fcntl(pwrdn_fd, F_SETOWN, getpid());
        // fcntl(pwrdn_fd, F_SETFL, fcntl(pwrdn_fd, F_GETFL) | FASYNC);

        //-----------------
        log_pack temp_log ={.log_level=1,.log_source = TempTask};

        while(temp_close & app_close) {

                pthread_kill(ppthread_pckto->main_task,TEMPERATURE_HB_SIG);
                pthread_mutex_lock(&temperature_mutex);
                while(temperature_flag == 0) {
                        pthread_cond_wait(&temperature_cond_var,&temperature_mutex);
                }

                pthread_mutex_unlock(&temperature_mutex);
                temperature_flag = 0;
                temperature_read(temp,temp_data); 
                data_cel = temp_conv(CELCIUS,temp_data);


                time_t t = time(NULL); 
                struct tm *tm = localtime(&t);
                strcpy(temp_log.time_stamp, asctime(tm));
                sprintf(data_cel_str,"temperature %f", data_cel);
                strcpy(temp_log.log_msg, data_cel_str);

                clock_gettime(CLOCK_MONOTONIC,&current);
                timeout.tv_sec = current.tv_sec+2;
                timeout.tv_nsec = current.tv_nsec;
                nbytes = mq_timedsend(message_queue,
                                         (const char*)&temp_log,
                                         sizeof(log_pack),
                                         message_priority,
                                         &timeout);
                if(nbytes<0) 
                {
                    ErrorHandler("TemperatureTask msg to log q", messagequeue_error,message_queue,error);
                }

                if(IPC_Flag == 1) {
                        IPC_Flag = 0;
                        clock_gettime(CLOCK_MONOTONIC,&current);
                        timeout.tv_sec = current.tv_sec+2;
                        timeout.tv_nsec = current.tv_nsec;
                        nbytes = mq_timedsend(IPCmessage_queue,(const char*)&temp_log,sizeof(log_pack),IPCmessage_priority,&timeout);

                        if(nbytes<0) {
                            ErrorHandler("TemperatureTask msqto log q error",messagequeue_error,message_queue,error);
                        }
                        else 
                            printf("data put on IPC msg Q\n");
                }


        }
        mq_close(message_queue);
        mq_close(messagequeue_error);
        mq_close(IPCmessage_queue);

        return NULL;
}
