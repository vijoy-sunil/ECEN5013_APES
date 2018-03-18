#include "main.h"

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

#ifdef BBB
  ret = system("echo none >/sys/class/leds/beaglebone:green:usr0/trigger");
  if (ret == -1)
    perror("LED CONTROL ERROR\n");
  else
    perror("LED OFF FROM HB \n");
  LED_OPTION(0);
#endif


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
  sigset_t mask, all_signals_mask; // set of signals
  
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

  alertmsg_queue =  mq_open(MSGQ_ALERT,O_CREAT | O_RDWR, S_IRWXU, &msgq_attr_err);

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

  uint8_t read_bytes;
  char choice;
  uint8_t light_cancelled = 0;
  uint8_t temp_cancelled = 0;
  uint8_t log_cancelled = 0;
  uint8_t socket_cancelled = 0;
  SLEEP(1); // allow other threads to initialize

  // required masks for main
  sigemptyset(&mask);
  sigaddset(&mask, SIGTEMP);
  sigaddset(&mask, SIGLIGHT);
  sigaddset(&mask, SIGTEMP_IPC);
  sigaddset(&mask, SIGLIGHT_IPC);
  sigaddset(&mask, SIGLOG);
  sigaddset(&mask, SIGCONT);

  ret =
      pthread_sigmask(SIG_SETMASK, // block the signals in the set argument
                      &mask,       // set argument has list of blocked signals
                      NULL); // if non NULL prev val of signal mask stored here
  if (ret == -1) {
    printf("Main pthread_sigmask:%s\n", strerror(errno));
    return -1;
  }

  printf("\n*******************************************************************"
         "\n");
  printf(
      " Enter thread to close 1-temperature 2-light 3-log 4-socket 5-application\n");
  printf(
      "*******************************************************************\n");

  while (application_close_flag) {

    // check HB signals every 5 seconds
    SLEEP(5);
    //  pthread_kill(socket,SIGCONT);//wake the socket to get its HB

    printf("M");

    if (light_cancelled == 0) {
      if (light_heartbeat_flag == 0)
        printf("NO HB from Light Task\n");
      else {
        printf("L");
        light_heartbeat_flag = 0;
      }
    }

    if (temp_cancelled == 0) {
      if (temperature_heartbeat_flag == 0)
        printf("NO HB from Temp Task\n");
      else {
        printf("T");
        temperature_heartbeat_flag = 0;
      }
    }

    if (log_cancelled == 0) {
      if (logger_heartbeat_flag == 0)
        printf("NO HB from Log Task\n");
      else {
        printf("l");
        logger_heartbeat_flag = 0;
      }
    }
    if (socket_cancelled == 0) {
      if (socket_heartbeat_flag == 0)
        printf("NO HB from Socket Task\n");
      else {
        printf("S*");
        socket_heartbeat_flag = 0;
      }
    }
    fflush(stdout);

    read_bytes = read(0, &choice, sizeof(char));
    if (read_bytes == 1) {
      printf("\nchoice:%c\n", choice);
      switch (choice) {
      case '1':
        if (temp_cancelled == 0) {
          printf("sending close signal to temperature task\n");
          temperature_close_flag = 0;
          temp_cancelled = 1;
        }
        break;
      case '2':
        if (light_cancelled == 0) {
          printf("sending close signal to light task\n");
          light_close_flag = 0;
          light_cancelled = 1;
        }
        break;
      case '3':
        if (log_cancelled == 0) {
          printf("sending close signal to log task\n");
          logger_close_flag = 0;
          log_cancelled = 1;
        }
        break;
      case '4':
        if (socket_cancelled == 0) {
          printf("sending close signal to socket task\n");
          socket_close_flag = 0;
          pthread_kill(socket, SIGCONT);
          socket_cancelled = 1;
        }
        break;

      case '5':
        printf("Closing application\n");
        temperature_close_flag = 0;
        light_close_flag = 0;
        logger_close_flag = 0;
        socket_close_flag = 0;
        pthread_kill(socket, SIGCONT);
        // pthread_cancel(temperature); pthread_cancel(light);
        // pthread_cancel(log);
        application_close_flag = 0;
        break;
      }
      read_bytes = 0;
    }
  }
  pthread_join(temperature, NULL);
  pthread_join(light, NULL);
  pthread_join(log, NULL);
  printf("Joined all threads\n");
  mq_close(alertmsg_queue);

  /*********destroy message Ques***********************/
  mq_unlink(IPC_TEMP_MQ);
  mq_unlink(IPC_LIGHT_MQ);
  mq_unlink(LOGGER_MQ);
  mq_unlink(MSGQ_ALERT);
  pthread_mutex_destroy(&light_lock);
  pthread_mutex_destroy(&lock_i2c);

  free(message_packet);
  free(fileid);
  printf("Destroyed all opened Msg Ques\n");

  printf("***************Exiting***************\n");
  return 0;
}

void socket_heartbeat_handl(int sig) {
  if (sig == SOCKET_SIG_HEARTBEAT) {
    socket_heartbeat_flag = 1;
  }
}

void light_hearbeat_handl(int sig) {
  if (sig == LIGHT_SIG_HEARTBEAT) {
    //  printf("L");
    light_heartbeat_flag = 1;
  }
}

void temperature_heartbeat_handl(int sig) {
  if (sig == TEMPERATURE_SIG_HEARTBEAT) {
    //  printf("T");
    temperature_heartbeat_flag = 1;
  }
}

void logger_heartbeat_handl(int sig) {
  if (sig == LOGGER_SIG_HEARTBEAT) {
    //    printf("l");
    logger_heartbeat_flag = 1;
  }
}