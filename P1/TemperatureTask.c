#include "TemperatureTask.h"

#define GET_TIME                                                               \
  clock_gettime(CLOCK_MONOTONIC, &now);                                        \
  expire.tv_sec = now.tv_sec + 2;                                              \
  expire.tv_nsec = now.tv_nsec;

sig_atomic_t temp_IPC_flag;

void TemptIPChandler(int sig) {
  if (sig == SIGTEMP_IPC) {
    printf("Caught signal TemptIPChandler\n");
    temp_IPC_flag = 1;
  }
}

void *TemperatureTask(void *pthread_inf) {

  uint8_t init_state = 1;
  char init_message[8][sizeof(alert_message)];
  temp_IPC_flag = 0;
  int ret;
  threadTaskAttr *ppthread_info = (threadTaskAttr *)pthread_inf;

  /*******Initialize ERROR Message Que*****************/
  mqd_t alertmsg_queue;
  int msg_prio_err = ERROR_MESSAGE_PRIORITY;
  int error_no;
  struct mq_attr msgq_attr_err = {.mq_maxmsg = MESSAGEQ_SIZE, // max # msg in queue
                                  .mq_msgsize =
                                      BUFFER_SIZE, // max size of msg in bytes
                                  .mq_flags = 0};

  alertmsg_queue =
      mq_open(MSGQ_ALERT,        // name
              O_CREAT | O_RDWR, // flags. create a new if dosent already exist
              S_IRWXU,          // mode-read,write and execute permission
              &msgq_attr_err);  // attribute

  if (alertmsg_queue < 0) {
    init_state = 0;
    sprintf(&(init_message[0][0]), "Temp Task mq_open-notify_mq  %s\n",
            strerror(errno));
  } else {
    sprintf(&(init_message[0][0]), "Temp Task mq_open-notify_mq %s\n",
            strerror(errno));
  }

  /******set periodic timer**************/
  ret = setTempTimer(); // sets up timer to periodically signal and wake this
                        // thread
  if (ret == -1) {
    init_state = 0;
    sprintf(&(init_message[1][0]), "Temptask-setTempTimer Error\n");
  } else {
    sprintf(&(init_message[1][0]), "Temptask-setTempTimer Success\n");
  }

  ret = pthread_mutex_init(&gtemp_mutex, NULL);
  if (ret == -1) {
    init_state = 0;
    sprintf(&(init_message[2][0]), "Temptask-pthread_mutex_init %s\n",
            strerror(errno));
  } else {
    sprintf(&(init_message[2][0]), "Temptask-pthread_mutex_init:%s\n",
            strerror(errno));
  }
  /*******Initialize Logger Message Que*****************/
  mqd_t logger_msgq;
  int msg_prio = 30;
  int num_bytes;
  char message[BUFFER_SIZE];
  struct mq_attr msgq_attr = {.mq_maxmsg = MESSAGEQ_SIZE, // max # msg in queue
                              .mq_msgsize =
                                  BUFFER_SIZE, // max size of msg in bytes
                              .mq_flags = 0};

  logger_msgq =
      mq_open(LOGGER_MQ,        // name
              O_CREAT | O_RDWR, // flags. create a new if dosent already exist
              S_IRWXU,          // mode-read,write and execute permission
              &msgq_attr);      // attribute

  if (logger_msgq < 0) {
    init_state = 0;
    sprintf(&(init_message[3][0]), "Temp Task-mq_open-loggerQ %s\n",
            strerror(errno));
  } else {
    sprintf(&(init_message[3][0]), "Temp Task-mq_open-loggerQ %s\n",
            strerror(errno));
  }
  /***************setting logger_msgq for IPC data Request******************/
  mqd_t IPCmsgq;
  int IPCmsg_prio = 20;
  int IPCnum_bytes;
  char IPCmessage[BUFFER_SIZE];

  struct mq_attr IPCmsgq_attr = {.mq_maxmsg = MESSAGEQ_SIZE, // max # msg in queue
                                 .mq_msgsize =
                                     BUFFER_SIZE, // max size of msg in bytes
                                 .mq_flags = 0};

  IPCmsgq =
      mq_open(IPC_TEMP_MQ,      // name
              O_CREAT | O_RDWR, // flags. create a new if dosent already exist
              S_IRWXU,          // mode-read,write and execute permission
              &IPCmsgq_attr);   // attribute
  if (IPCmsgq < 0) {
    init_state = 0;
    sprintf(&(init_message[4][0]), "Temptask-mq_open-IPCQ %s\n",
            strerror(errno));
  } else {
    sprintf(&(init_message[4][0]), "Temptask-mq_open-IPCQ %s\n",
            strerror(errno));
  }
  // set up the signal to request data
  struct sigaction sigactn;
  sigemptyset(&sigactn.sa_mask);
  sigactn.sa_handler = TemptIPChandler;
  ret = sigaction(SIGTEMP_IPC, &sigactn, NULL);

  if (ret == -1) {
    init_state = 0;
    sprintf(&(init_message[5][0]), "Temp Task sigaction %s\n", strerror(errno));
  } else {
    sprintf(&(init_message[5][0]), "Temp task sigaction %s\n", strerror(errno));
  }

/************Initialize temperatue sensor******************/
#ifdef BBB
  int temperature = initializeTemp();
  char temp_data[2], data_cel_str[BUFFER_SIZE - 200];
  float data_cel;

  if (temperature == -1) {
    init_state = 0;
    sprintf(&(init_message[6][0]), "Temp Sensor init Error\n");
  } else {
    sprintf(&(init_message[6][0]), "Temp Sensor init Success\n");
  }
#endif

  /*****************Mask SIGNALS********************/
  sigset_t mask; // set of signals
  sigemptyset(&mask);
  sigaddset(&mask, SIGLIGHT);
  sigaddset(&mask, LIGHT_SIG_HEARTBEAT);
  sigaddset(&mask, LOGGER_SIG_HEARTBEAT);
  sigaddset(&mask, TEMPERATURE_SIG_HEARTBEAT);
  sigaddset(&mask, SIGLOG);
  sigaddset(&mask, SIGCONT);
  sigaddset(&mask, SOCKET_SIG_HEARTBEAT);

  ret =
      pthread_sigmask(SIG_SETMASK, // block the signals in the set argument
                      &mask,       // set argument has list of blocked signals
                      NULL); // if non NULL prev val of signal mask stored here
  if (ret == -1) {
    init_state = 0;
    sprintf(&(init_message[7][0]), "Temp task-pthread_sigmask %s\n",
            strerror(errno));
  } else {
    sprintf(&(init_message[7][0]), "Temp task-pthread_sigmask %s\n",
            strerror(errno));
  }

  // send initialization status
  notify(&init_message[0][0], alertmsg_queue, logger_msgq, init);
  notify(&init_message[1][0], alertmsg_queue, logger_msgq, init);
  notify(&init_message[2][0], alertmsg_queue, logger_msgq, init);
  notify(&init_message[3][0], alertmsg_queue, logger_msgq, init);
  notify(&init_message[4][0], alertmsg_queue, logger_msgq, init);
#ifdef BBB
  notify(&init_message[5][0], alertmsg_queue, logger_msgq, init);
#endif
  notify(&init_message[6][0], alertmsg_queue, logger_msgq, init);
  notify(&init_message[7][0], alertmsg_queue, logger_msgq, init);

  if (init_state == 0) {
    notify("##All elements not initialized in Temp Task, Not proceeding with "
           "it##\n",
           alertmsg_queue, logger_msgq, error);
    while (temperature_close_flag & application_close_flag) {
      sleep(1);
    };
    return NULL;
  }

  else if (init_state == 1)
    notify("##All elements initialized in Temp Task, proceeding with it##\n",
           alertmsg_queue, logger_msgq, init);

  /************Creating logpacket*******************/
  log_pack temp_log = {.log_level = 1, .log_source = temperatue_Task};
  struct timespec now, expire;

#ifdef BBB
  /*****Looging BBB configurations*******/
  char buffer[3];
  tlowRead(temperature, buffer);
  printf("TEMP SENSOR TLOW READ %x %x \n", buffer[0], buffer[1]);

  thighWrite(temperature, buffer);
  buffer[0] = buffer[1] = 0;
  thighRead(temperature, buffer);
  printf("TEMP SENSOR THIGH READ %x %x \n", buffer[0], buffer[1]);

  buffer[0] = TEMP_CONFIG_REG;
  buffer[1] = SHUTDOWN_DI;
  configRegWrite(temperature, buffer);

  printf("TEMP SENSOR BEFORE SHUTDOWN %x %x \n", buffer[0], buffer[1]);

  buffer[0] = TEMP_CONFIG_REG;
  buffer[1] = SHUTDOWN_EN;

  configRegWrite(temperature, buffer);

  buffer[0] = buffer[1] = 0;
  configRegRead(temperature, buffer);

  printf("TEMP SENSOR AFTER SHUTDOWN %x %x \n", buffer[0], buffer[1]);

  buffer[0] = TEMP_CONFIG_REG;
  buffer[1] = SHUTDOWN_DI;
  configRegWrite(temperature, buffer);

  buffer[0] = TEMP_CONFIG_REG;
  buffer[1] = RES_10BIT;
  buffer[2] = EMMODE | CONVRATE3;

  configRegWrite(temperature, buffer);

  buffer[0] = buffer[1] = 0;
  configRegRead(temperature, buffer);

  printf("TEMPSENSOR 10 BIT RESOLUTION, EMMODE AND CONV RATE 8Hz- %x %x \n",
         buffer[0], buffer[1]);

  printf("FAULT BITS ARE %d %d\n", (buffer[0] & 0x08) >> 3,
         (buffer[0] & 0x10) >> 4);
#endif

  /****************Do this periodically*******************************/
  while (temperature_close_flag & application_close_flag) {

    // wait for next second
    pthread_mutex_lock(&gtemp_mutex);
    while (gtemp_flag == 0) {
      pthread_cond_wait(&gtemp_condition, &gtemp_mutex);
    }

    pthread_mutex_unlock(&gtemp_mutex);
    gtemp_flag = 0;
    // send HB
    pthread_kill(ppthread_info->main, TEMPERATURE_SIG_HEARTBEAT);
// collect temperatue
#ifdef BBB
    temperatureRead(temperature, temp_data);
    data_cel = temperatureConv(temp_format, temp_data);

    /************populate the log packet*********/
    sprintf(data_cel_str, "temperature %f", data_cel);
    strcpy(temp_log.log_msg, data_cel_str);
#else
    strcpy(temp_log.log_msg, "Mock temperature");

#endif
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strcpy(temp_log.time_stamp, asctime(tm));

    /*******Log messages on Que*************/
    // set up time for timed send

    clock_gettime(CLOCK_MONOTONIC, &now);
    expire.tv_sec = now.tv_sec + 2;
    expire.tv_nsec = now.tv_nsec;
    num_bytes = mq_timedsend(logger_msgq, (const char *)&temp_log,
                             sizeof(log_pack), msg_prio, &expire);
    if (num_bytes < 0) {
      notify("mq_send to Log Q in TemperatureTask", alertmsg_queue, logger_msgq, error);
    }
    /******Log data on IPC Que if requested******/

    if (temp_IPC_flag == 1) {
      temp_IPC_flag = 0;
      // set up time for timed send
      clock_gettime(CLOCK_MONOTONIC, &now);
      expire.tv_sec = now.tv_sec + 2;
      expire.tv_nsec = now.tv_nsec;
      num_bytes = mq_timedsend(IPCmsgq, (const char *)&temp_log,
                               sizeof(log_pack), IPCmsg_prio, &expire);
      if (num_bytes < 0) {
        notify("mq_send-IPC Q-TemperatureTask Error", alertmsg_queue, logger_msgq, error);
      }
    }
    // printf("hi\n");
    //  notify("Test Error",alertmsg_queue,logger_msgq,error);
  }
  printf("exiting Temp task\n");
  mq_close(logger_msgq);
  mq_close(alertmsg_queue);
  mq_close(IPCmsgq);

  return NULL;
}
