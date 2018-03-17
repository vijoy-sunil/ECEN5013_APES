#include "main.h"

void LightHBhandler(int sig);
void TempHBhandler(int sig);
void LogHBhandler(int sig);

int main()
{
        int rc;
        message_packet = (err_message_packet*)malloc(sizeof(err_message_packet));
        if(message_packet == NULL) {perror("malloc-main"); return -1;}

        app_close = 1; 
        light_close = 1; 
        temp_close = 1; 
        logger_close = 1;
        temperature_heartbeat_flag = 0; light_heartbeat_flag = 0;


//Setup error msg
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
        if(messagequeue_error < 0) 
        {
            perror("mq_open-error_mq-tempTask "); 
            return -1;
        }
        else 
            printf("Messgae Que Opened in tempTask\n");

        //Close program by pressing Ctrl+C
        signal(SIGINT,Signal_interrupt);

//set up notification for this error

        sig_ev_err.sigev_notify=SIGEV_THREAD;      //notify by signal in sigev_signo
        sig_ev_err.sigev_notify_function = errorFunction;
        sig_ev_err.sigev_notify_attributes=NULL;
        sig_ev_err.sigev_value.sival_ptr=&messagequeue_error;  //data passed with notification


        rc  = mq_notify(messagequeue_error,&sig_ev_err);
        if(rc == -1) {perror("mq_notify-main"); return -1;}



/*******************Masking SIgnals***********************/
        sigset_t mask; //set of signals
        sigemptyset(&mask);
        sigaddset(&mask,TEMPERATURE_SIGNAL); sigaddset(&mask,LIGHT_SIGNAL);
        sigaddset(&mask,TEMPERATURE_SIGNAL_IPC); sigaddset(&mask,LIGHT_SIGNAL_IPC);
        sigaddset(&mask,LOGGER_SIGNAL);

        //read the status of global variables when wake up from sleep
        //sigaddset(&mask,LOGGER_HB_SIG); sigaddset(&mask,TEMPERATURE_HB_SIG);

        rc = pthread_sigmask(
                SIG_SETMASK, //block the signals in the set argument
                &mask, //set argument has list of blocked signals
                NULL); //if non NULL prev val of signal mask stored here
        if(rc == -1) { printf("Error:%s\n",strerror(errno)); return -1; }

        pthread_t temp,light,log;
        threadPckt temp_info; temp_info.t_id = 1; temp_info.main_task=pthread_self();
        threadPckt light_info; light_info.t_id = 2; light_info.main_task=pthread_self();
        threadPckt log_info; log_info.t_id = 3; log_info.main_task=pthread_self();

//Register Light HB signal
        struct sigaction action;
        sigemptyset(&action.sa_mask);
        action.sa_handler = LightHBhandler;
        rc = sigaction(LIGHT_HB_SIG,&action,NULL);
        if(rc == -1) { perror("sigaction main"); return -1; }

//Register Temp HB signal
        sigemptyset(&action.sa_mask);
        action.sa_handler = TempHBhandler;
        rc = sigaction(TEMPERATURE_HB_SIG,&action,NULL);
        if(rc == -1) { perror("sigaction main"); return -1; }

//Register Log HB signal
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        action.sa_handler = LogHBhandler;
        rc = sigaction(LOGGER_HB_SIG,&action,NULL);
        if(rc == -1) { perror("sigaction main"); return -1; }



        rc = pthread_create(  &temp,
                               ((void *)0),
                               tempTask,
                               (void *)&(temp_info) );
        if (rc != 0) {  printf("Error:%s\n",strerror(errno)); return -1;}

        rc = pthread_create(  &light,
                               ((void *)0),
                               lightTask,
                               (void *)&(light_info) );
        if (rc != 0) {  printf("Error:%s\n",strerror(errno)); return -1;}

        rc = pthread_create(  &log,
                               ((void *)0),
                               logTask,
                               (void *)&(log_info) );
        if (rc != 0) {  printf("Error:%s\n",strerror(errno)); return -1;}
        uint8_t read_bytes; char choice;
        uint8_t light_close=0; uint8_t close_temperature=0; uint8_t close_logger=0;
        SLEEP(2);//allow other threads to initialize
        printf(" Enter thread to close 1-temp; 2-light; 3-log; 4-application\n");

        while (app_close) {

//check HB signals

                printf("M");

                if(light_close == 0) {
                        if(light_heartbeat_flag == 0) printf("NO HB from Light Task\n");
                        else {printf("L"); light_heartbeat_flag = 0;}
                }

                if(close_temperature == 0) {
                        if(temperature_heartbeat_flag == 0) printf("NO HB from Temp Task\n");
                        else {printf("T"); temperature_heartbeat_flag = 0;}
                }

                if(close_logger == 0) {
                        if(logger_heartbeat_flag == 0) printf("NO HB from Log Task\n");
                        else {printf("l*"); logger_heartbeat_flag = 0;}
                }
                fflush(stdout);

                read_bytes=read(0,&choice,sizeof(char));
                if(read_bytes == 1) {
                        printf("\nchoice:%c\n",choice);
                        switch(choice) {
                        case '1':
                                if(close_temperature == 0) {
                                        printf("sending close signal to temp task\n");
                                        temp_close = 0; close_temperature = 1;
                                }
                                break;
                        case '2':
                                if(light_close == 0) {
                                        printf("sending close signal to light task\n");
                                        light_close = 0; light_close = 1;
                                }
                                break;
                        case '3':
                                if(close_logger == 0) {
                                        printf("sending close signal to log task\n");
                                        logger_close = 0; close_logger = 1;
                                }
                                break;
                        case '4':
                                printf("Closing application\n");
                                temp_close = 0; light_close = 0;  logger_close = 0;
                                //pthread_cancel(temp); pthread_cancel(light);
                                //pthread_cancel(log);
                                app_close = 0;
                                break;
                        }
                        read_bytes = 0;
                }

//                sleep(5)
                SLEEP(5);
        }
        pthread_join(temp, NULL);
        pthread_join(light, NULL);
        pthread_join(log, NULL);
        printf("Joined all threads\n");
        mq_close(messagequeue_error);

/*********destroy message Ques***********************/
        mq_unlink(IPC_TEMP_MQ);
        mq_unlink(IPC_LIGHT_MQ);
        mq_unlink(MY_MQ);
        mq_unlink(MY_MQ_ERR);
        free(message_packet);

        printf("All tasks closed and queues closed\n");

        printf("Exit program\n");
        return 0;
}
void LightHBhandler(int sig){
        if(sig == LIGHT_HB_SIG)
        {
                light_heartbeat_flag = 1;
        }
}

void TempHBhandler(int sig){
        if(sig == TEMPERATURE_HB_SIG)
        {
                temperature_heartbeat_flag = 1;
        }
}

void LogHBhandler(int sig){
        if(sig == LOGGER_HB_SIG)
        {
                logger_heartbeat_flag = 1;
        }
}