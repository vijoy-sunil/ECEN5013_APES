/***************************************************************
* AUTHOR  : Praveen Gnanaseakran
* DATE    : 03/08/2018
* DESCRITPTION  : main files
                  
* HEADER FILES  : main.h
****************************************************************/

#include "main.h"

/*****************interrupt handler***************************/
void pwrdn_sighandler(int signo)
{
	if (signo==SIGIO) {
		printf("\n * * * * * * * * temperature interrupt called * * * * * \n");
		INTR_LED_ON;
	}
	return;
}
/************************************************************/
int main(int argc, char *argv[]) {
  if (argc > 1) {
    fileid = (char *)malloc(sizeof(argv[1]));
    strcpy(fileid, argv[1]);
  } else {
    fileid = (char *)malloc(sizeof(DEFAULT_FILE_NAME));
    strcpy(fileid, DEFAULT_FILE_NAME);
  }
  
  printf("Enter Temp Format C-CELCIUS  F-FARENHEIT  K-KELVIN\n");
  
  char input;
  input=getc(stdin);

  if (input == 'C')
  {
    printf("CELCIUS\n");
    temp_format = CELCIUS;
  }
  else if (input == 'F')
  {
    printf("FARENHEIT\n");
    temp_format = FARENHEIT;
  }
  else if (input == 'K')
  {
    printf("KELVIN\n");
    temp_format = KELVIN;
  }
  else
  {
    printf("DEFAULT CELCIUS\n");
    input = CELCIUS;
  }


  printf("LOGFILE name %s\n", fileid);

  int ret;

  application_close_flag = 1;
  light_close_flag = 1;
  temperature_close_flag = 1;
  logger_close_flag = 1;
  socket_close_flag = 1;
  temperature_heartbeat_flag = 0;
  light_heartbeat_flag = 0;
  logger_heartbeat_flag = 0;
  socket_heartbeat_flag = 0;

  message_packet = (alert_message *)malloc(sizeof(alert_message));

  ret = system("echo none >/sys/class/leds/beaglebone:green:usr0/trigger");
  if (ret == -1)
    perror("LED CONTROL ERROR\n");
  else
    perror("LED OFF FROM HB \n");
  LED_OPTION(0);


  ret = pthread_mutex_init(&light_lock, NULL);

  if (ret == -1)
    perror("light_lock failed");

  ret = pthread_mutex_init(&lock_i2c, NULL);
  if (ret == -1)
    perror("lock_i2c failed");

  if (message_packet == NULL) {
    perror("malloc failed");
    return -1;
  }

//MASKING SIGNALS
  sigset_t mask_bit, all_signals_mask; // set of signals
  
  sigfillset(&all_signals_mask);
  
  ret =pthread_sigmask(SIG_SETMASK, &all_signals_mask, NULL); 

  if (ret == -1) 
  {
    perror("MASKING ERROR");
    return -1;
  }


  signal(SIGINT, SIGNAL_INTERRUPT_HANDL);


  mqd_t alertmsg_queue;
  int msg_prio_err = ERROR_MESSAGE_PRIORITY;
  int error_no;
  struct mq_attr msgq_attr_err = {.mq_maxmsg = MESSAGEQ_SIZE, .mq_msgsize =BUFFER_SIZE, .mq_flags = 0};

  alertmsg_queue =  mq_open(ALERT_MSGQ_PCKT,O_CREAT | O_RDWR, S_IRWXU, &msgq_attr_err);

  if (alertmsg_queue < 0) {
    perror("Msq q error");
    return -1;
  }

  signal_event.sigev_notify = SIGEV_THREAD; 
  signal_event.sigev_notify_function = alertReceive;
  signal_event.sigev_notify_attributes = NULL;
  signal_event.sigev_value.sival_ptr = &alertmsg_queue; 


  ret = mq_notify(alertmsg_queue, &signal_event);
  if (ret == -1) 
  {
    perror("Alert error");
    return -1;
  }

  struct sigaction sigactn;
  sigemptyset(&sigactn.sa_mask);
  sigactn.sa_handler = light_hearbeat_handl;
  ret = sigaction(LIGHT_SIG_HEARTBEAT, &sigactn, NULL);
  if (ret == -1) {
    perror("SIGACTION ERROR");
    return -1;
  }

  sigemptyset(&sigactn.sa_mask);
  sigactn.sa_handler = temperature_heartbeat_handl;
  ret = sigaction(TEMPERATURE_SIG_HEARTBEAT, &sigactn, NULL);
  if (ret == -1) {
    perror("SIGACTION ERROR");
    return -1;
  }

  sigemptyset(&sigactn.sa_mask);
  sigactn.sa_flags = 0;
  sigactn.sa_handler = logger_heartbeat_handl;
  ret = sigaction(LOGGER_SIG_HEARTBEAT, &sigactn, NULL);
  if (ret == -1) {
    perror("SIGACTION ERROR");
    return -1;
  }

  sigemptyset(&sigactn.sa_mask);
  sigactn.sa_flags = 0;
  sigactn.sa_handler = socket_heartbeat_handl;
  ret = sigaction(SOCKET_SIG_HEARTBEAT, &sigactn, NULL);
  if (ret == -1) {
    perror("SIGACTION ERROR");
    return -1;
  }

  /******************interrupt signalling******************/
	READY_LED;

	int pwrdn_fd;
	int count;
	struct sigaction intr_action;
	
	memset(&intr_action, 0, sizeof(intr_action));
	intr_action.sa_handler = pwrdn_sighandler;
	intr_action.sa_flags = 0;

	sigaction(SIGIO, &intr_action, NULL);

	pwrdn_fd = open("/dev/gpio_int", O_RDWR);

	if (pwrdn_fd < 0) {
	printf("Failed to open device\n");
	//return 1;
	}

	fcntl(pwrdn_fd, F_SETOWN, getpid());
	fcntl(pwrdn_fd, F_SETFL, fcntl(pwrdn_fd, F_GETFL) | FASYNC);	
  /********************************************************/

  pthread_t temperature, light, log, socket;
  threadTaskAttr temp_task_info;
  temp_task_info.t_id = 1;
  temp_task_info.main = pthread_self();
  threadTaskAttr light_task_info;
  light_task_info.t_id = 2;
  light_task_info.main = pthread_self();
  threadTaskAttr log_task_info;
  log_task_info.t_id = 3;
  log_task_info.main = pthread_self();

  threadTaskAttr socket_info;
  socket_info.t_id = 3;
  socket_info.main = pthread_self();

  ret = pthread_create(&log, DEFAULT_THREAD_ATTR, logTask, (void *)&(log_task_info));
  if (ret != 0) {
    printf("Pthread error:%s\n", strerror(errno));
    return -1;
  }
  sleep(1);
  ret = pthread_create(&light, DEFAULT_THREAD_ATTR, LightTask,
                       (void *)&(light_task_info));
  if (ret != 0) {
    printf("Pthread error:%s\n", strerror(errno));
    return -1;
  }
  sleep(1);
  ret = pthread_create(&temperature, DEFAULT_THREAD_ATTR, TemperatureTask,
                       (void *)&(temp_task_info));
  if (ret != 0) {
    printf("Pthread error:%s\n", strerror(errno));
    return -1;
  }
  sleep(1);
  ret = pthread_create(&socket, DEFAULT_THREAD_ATTR, socketTask,
                       (void *)&(socket_info));
  if (ret != 0) {
    printf("Pthread error:%s\n", strerror(errno));
    return -1;
  }

  uint8_t bytesread;
  char threadoption;
  uint8_t light_exit_flag = 0;
  uint8_t temp_exit_flag = 0;
  uint8_t logger_exit_flag = 0;
  uint8_t socket_exit_flag = 0;

  UNITERRUPTIBLE_SLEEP(1); // allow other threads to initialize

  // required masks for main
  sigemptyset(&mask_bit);
  sigaddset(&mask_bit, TEMPERATURE_SIGNAL_OPT);
  sigaddset(&mask_bit, LIGHT_SIGNAL_OPT);
  sigaddset(&mask_bit, TEMPSIGNAL_PACKET);
  sigaddset(&mask_bit, LIGHTSIGNAL_PACKET);
  sigaddset(&mask_bit, LOGGER_SIG);
  sigaddset(&mask_bit, SIGCONT);

  ret =pthread_sigmask(SIG_SETMASK, &mask_bit, NULL); // if non NULL prev val of signal mask_bit stored here
  if (ret == -1) {
    printf("Main pthread_sigmask:%s\n", strerror(errno));
    return -1;
  }
  char killoption;
  printf("KILL A TASK: T-temperature L-light G-log S-socket A-application\n");

  while (application_close_flag) {

    // check HB signals every 5 seconds for 5 tasks
    UNITERRUPTIBLE_SLEEP(5);

    if (light_exit_flag == 0) {
      if (light_heartbeat_flag == 0)
        printf("Light task no heartbeat\n");
      else {
        printf("Light task alive\n");
        light_heartbeat_flag = 0;
      }
    }

    if (temp_exit_flag == 0) {
      if (temperature_heartbeat_flag == 0)
        printf("Temperature task no heartbeat\n");
      else {
        printf("Temperature task alive\n");
        temperature_heartbeat_flag = 0;
      }
    }

    if (logger_exit_flag == 0) {
      if (logger_heartbeat_flag == 0)
        printf("Logger task no heartbeat\n");
      else {
        printf("Logger task alive\n");
        logger_heartbeat_flag = 0;
      }
    }
    if (socket_exit_flag == 0) {
      if (socket_heartbeat_flag == 0)
        printf("Socket task no heartbeat\n");
      else {
        printf("Socket task alive\n");
        socket_heartbeat_flag = 0;
      }
    }
    fflush(stdout);

    killoption=getc(stdin);
      if(killoption=='T'){
        if (temp_exit_flag == 0) {
          printf("CLOSING LIGHT TASK\n");
          temperature_close_flag = 0;
          temp_exit_flag = 1;
        }
      }
      if(killoption=='L'){
        if (light_exit_flag == 0) {
          printf("CLOSING LIGHT TASK\n");
          light_close_flag = 0;
          light_exit_flag = 1;
        }
      }
      if(killoption=='G'){
        if (logger_exit_flag == 0) {
          printf("CLOSING LOGGER TASK\n");
          logger_close_flag = 0;
          logger_exit_flag = 1;
        }
      }
      if(killoption=='S'){
        if (socket_exit_flag == 0) {
          printf("CLOSING SOCKET TASK\n");
          socket_close_flag = 0;
          pthread_kill(socket, SIGCONT);
          socket_exit_flag = 1;
        }
      }

      if(killoption=='A'){
        printf("ALL TASKS BEING CLOSED in 5 seconds\n");
        temperature_close_flag = 0;
        light_close_flag = 0;
        logger_close_flag = 0;
        socket_close_flag = 0;
        pthread_kill(socket, SIGCONT);
        application_close_flag = 0;
      }
  }
  pthread_join(temperature, NULL);
  pthread_join(light, NULL);
  pthread_join(log, NULL);
  printf("Joining all threads\n");
  mq_close(alertmsg_queue);

  mq_unlink(TEMPERATURE_MSGQ_IPC);
  mq_unlink(LIGHT_MSGQ_IPC);
  mq_unlink(LOGGER_MSGQ_IPC);
  mq_unlink(ALERT_MSGQ_PCKT);
  pthread_mutex_destroy(&light_lock);
  pthread_mutex_destroy(&lock_i2c);

  free(message_packet);
  free(fileid);
  printf("Successfully closed all msgq\n");
  return 0;
}

void socket_heartbeat_handl(int sig) {
  if (sig == SOCKET_SIG_HEARTBEAT) {
    socket_heartbeat_flag = 1;
  }
}

void light_hearbeat_handl(int sig) {
  if (sig == LIGHT_SIG_HEARTBEAT) {
    light_heartbeat_flag = 1;
  }
}

void temperature_heartbeat_handl(int sig) {
  if (sig == TEMPERATURE_SIG_HEARTBEAT) {
    temperature_heartbeat_flag = 1;
  }
}

void logger_heartbeat_handl(int sig) {
  if (sig == LOGGER_SIG_HEARTBEAT) {
    logger_heartbeat_flag = 1;
  }
}
