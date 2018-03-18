/***************************************************************
* AUTHOR  : Vijoy Sunil Kumar
* DATE    : 03/08/2018
* DESCRITPTION  : Light Sensor Task
                  
* HEADER FILES  : LightTask.h
****************************************************************/

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
        int msg_priority = MESSAGE_PRIORITY;
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
        int msgprio_ipc = 20;
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
	uint16_t threshlow = 0;

	//--------------------------------------------------------------
	commandReg(light,THRESHLOWLOW,WRITE);
        lightbuffer[0] = 0x00;
	interruptThreshReg(light, WRITE, lightbuffer);

	
	commandReg(light,THRESHLOWHIGH,WRITE);
	lightbuffer[0] = 0x02;
	interruptThreshReg(light, WRITE, lightbuffer);
	

	commandReg(light,THRESHLOWLOW,WRITE);
	interruptThreshReg(light, READ, lightbuffer);

	threshlow = lightbuffer[0] << 8;
	commandReg(light,THRESHLOWHIGH,WRITE);
	interruptThreshReg(light, READ, lightbuffer);
	threshlow |= lightbuffer[0];

	printf("THRESHOLD LOW value :%x\n",threshlow);


	commandReg(light,THRESHHIGHLOW,WRITE);
	lightbuffer[0] = 0x00;
	interruptThreshReg(light, WRITE, lightbuffer);


	commandReg(light,THRESHHIGHIGH,WRITE);
	lightbuffer[0] = 0x0A;
	interruptThreshReg(light, WRITE, lightbuffer);


	uint16_t threshhigh = 0;
	commandReg(light,THRESHHIGHLOW,WRITE);
	interruptThreshReg(light, READ, lightbuffer);

	threshhigh = lightbuffer[0] << 8;
	commandReg(light,THRESHHIGHIGH,WRITE);
	interruptThreshReg(light, READ, lightbuffer);
	threshhigh |= lightbuffer[0];

	printf("THRESHOLD HIGH value :%x\n",threshhigh);

        commandReg(light,INTERRUPT,WRITE);
        interrupReg(light, READ, ENABLE, lightbuffer);
        printf("INTERRUPT CONTROL register value default:%x\n",lightbuffer[0]);

        commandReg(light,INTERRUPT,WRITE);
        interrupReg(light, WRITE, ENABLE, lightbuffer);
	interrupReg(light, READ, ENABLE, lightbuffer);
        printf("INTERRUPT CONTROL register value interrupt set:%x\n",lightbuffer[0]);      
	//-----------------------------------------------------------------

        sigset_t mask_bit; 
        sigemptyset(&mask_bit);
        sigaddset(&mask_bit,TEMPERATURE_SIGNAL_OPT); sigaddset(&mask_bit,TEMPERATURE_SIG_HEARTBEAT);
        sigaddset(&mask_bit,LOGGER_SIG); sigaddset(&mask_bit,LIGHT_SIG_HEARTBEAT);
        sigaddset(&mask_bit,LOGGER_SIG_HEARTBEAT); sigaddset(&mask_bit,SIGCONT);
        sigaddset(&mask_bit,SOCKET_SIG_HEARTBEAT);


        ret = pthread_sigmask(SIG_SETMASK,&mask_bit,NULL); 

        if(ret  == -1) {
                initialize =0;
                sprintf(&(initialize_msg[6][0]),"Failure sigmask %s\n",strerror(errno));
        }
        else {
                sprintf(&(initialize_msg[6][0]),"Success sigmask %s\n",strerror(errno));
        }

        alert(&initialize_msg[0][0],alertmsg_queue,logtask_msg_que,init);
        alert(&initialize_msg[1][0],alertmsg_queue,logtask_msg_que,init);
        alert(&initialize_msg[2][0],alertmsg_queue,logtask_msg_que,init);
        alert(&initialize_msg[3][0],alertmsg_queue,logtask_msg_que,init);
        alert(&initialize_msg[4][0],alertmsg_queue,logtask_msg_que,init);
        alert(&initialize_msg[5][0],alertmsg_queue,logtask_msg_que,init);
        alert(&initialize_msg[6][0],alertmsg_queue,logtask_msg_que,init);
        alert(&initialize_msg[7][0],alertmsg_queue,logtask_msg_que,init);

        if(initialize == 0) 
        { 
            alert("Light task faiulre quitting\n",alertmsg_queue,logtask_msg_que,error);
            while(light_close_flag & application_close_flag) 
            {
                sleep(1);
            };
          return NULL;
        }

        else if(initialize == 1) 

            alert("Light task success continuing\n",alertmsg_queue,logtask_msg_que,init);


        logger_pckt light_logmsg ={.log_level=1,.log_source = light_Task};
        struct timespec current,expire;


        while(light_close_flag & application_close_flag) {

                pthread_kill(pthread_info->main,LIGHT_SIG_HEARTBEAT);//send HB

                pthread_mutex_lock(&glight_mutex);
                while(glight_flag == 0) {
                        pthread_cond_wait(&glight_condition,&glight_mutex);
                }
                pthread_mutex_unlock(&glight_mutex);
                glight_flag = 0;

                ch0=LightSensorRead(light,0);
                ch1=LightSensorRead(light,1);
                lumen = LumenOut(ch0, ch1);

                sprintf(data_lumen_str,"Lumens %f", lumen);
                strcpy(light_logmsg.log_msg, data_lumen_str);


                time_t t = time(NULL); struct tm *tm = localtime(&t);
                strcpy(light_logmsg.time_stamp, asctime(tm));

                clock_gettime(CLOCK_MONOTONIC,&current);
                expire.tv_sec = current.tv_sec+2;
                expire.tv_nsec = current.tv_nsec;
                num_bytes = mq_timedsend(logtask_msg_que,(const char*)&light_logmsg,sizeof(logger_pckt),msg_priority,&expire);

                if(num_bytes<0) 
                {
                    alert("Light message queue sent", alertmsg_queue,logtask_msg_que,error);
                }

                if(day_night != reportStatus(light)) 
                {
                    day_night = reportStatus(light);
                    sprintf(data_lumen_str,"Day Night State change\n");
                    strcpy(light_logmsg.log_msg, data_lumen_str);

                    num_bytes = mq_timedsend(logtask_msg_que,(const char*)&light_logmsg,sizeof(logger_pckt),msg_priority,&expire);

                    if(num_bytes<0) 
                    {
                        alert("mq_send-Log Q-LightTask",alertmsg_queue,logtask_msg_que,error);
                    }
                }



                if(Light_Msg_flag == 1) 
                {
                    Light_Msg_flag = 0;

                    clock_gettime(CLOCK_MONOTONIC,&current);
                    expire.tv_sec = current.tv_sec+2;
                    expire.tv_nsec = current.tv_nsec;
                    num_bytes = mq_timedsend(messageq_ipc,(const char*)&light_logmsg,sizeof(logger_pckt),msgprio_ipc,&expire);

                    if(num_bytes<0) 
                    {
                        alert("mq_send-IPC-LightTask Error",alertmsg_queue,logtask_msg_que,error);
                    }
                }


        }
        printf("LIGHT TASK CLOSE\n");
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
