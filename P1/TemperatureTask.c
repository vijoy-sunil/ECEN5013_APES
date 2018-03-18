#include "TemperatureTask.h"


sig_atomic_t temp_IPC_flag;

void TemptIPChandler(int sig) {
  if (sig == TEMPSIGNAL_PACKET) {
    printf("Caught signal TemptIPChandler\n");
    temp_IPC_flag = 1;
  }
}

void *TemperatureTask(void *pthread_inf) {

  uint8_t initialize = 1;
  char initialize_msg[8][sizeof(alert_message)];
  temp_IPC_flag = 0;
  int ret;
  threadTaskAttr *pthread_info = (threadTaskAttr *)pthread_inf;

  /*******Initialize ERROR Message Que*****************/
  mqd_t alertmsg_queue;
  int msg_prio_err = ERROR_MESSAGE_PRIORITY;
  int error_no;
  struct mq_attr msgq_attr_err = {.mq_maxmsg = MESSAGEQ_SIZE, // max # msg in queue
                                  .mq_msgsize =
                                      BUFFER_SIZE, // max size of msg in bytes
                                  .mq_flags = 0};

  alertmsg_queue =
      mq_open(ALERT_MSGQ_PCKT,        // name
              O_CREAT | O_RDWR, // flags. create a new if dosent already exist
              S_IRWXU,          // mode-read,write and execute permission
              &msgq_attr_err);  // attribute

  if (alertmsg_queue < 0) {
    initialize = 0;
    sprintf(&(initialize_msg[0][0]), "Temp Task mq_open-notify_mq  %s\n",
            strerror(errno));
  } else {
    sprintf(&(initialize_msg[0][0]), "Temp Task mq_open-notify_mq %s\n",
            strerror(errno));
  }

  /******set periodic timer**************/
  ret = setTempTimer(); // sets up timer to periodically signal and wake this
                        // thread
  if (ret == -1) {
    initialize = 0;
    sprintf(&(initialize_msg[1][0]), "Temptask-setTempTimer Error\n");
  } else {
    sprintf(&(initialize_msg[1][0]), "Temptask-setTempTimer Success\n");
  }

  ret = pthread_mutex_init(&gtemp_mutex, NULL);
  if (ret == -1) {
    initialize = 0;
    sprintf(&(initialize_msg[2][0]), "Temptask-pthread_mutex_init %s\n",
            strerror(errno));
  } else {
    sprintf(&(initialize_msg[2][0]), "Temptask-pthread_mutex_init:%s\n",
            strerror(errno));
  }
  /*******Initialize Logger Message Que*****************/
  mqd_t logtask_msg_que;
  int msg_priority = 30;
  int num_bytes;
  char message[BUFFER_SIZE];
  struct mq_attr msgq_attr = {.mq_maxmsg = MESSAGEQ_SIZE, // max # msg in queue
                              .mq_msgsize =
                                  BUFFER_SIZE, // max size of msg in bytes
                              .mq_flags = 0};

  logtask_msg_que =
      mq_open(LOGGER_MSGQ_IPC,        // name
              O_CREAT | O_RDWR, // flags. create a new if dosent already exist
              S_IRWXU,          // mode-read,write and execute permission
              &msgq_attr);      // attribute

  if (logtask_msg_que < 0) {
    initialize = 0;
    sprintf(&(initialize_msg[3][0]), "Temp Task-mq_open-loggerQ %s\n",
            strerror(errno));
  } else {
    sprintf(&(initialize_msg[3][0]), "Temp Task-mq_open-loggerQ %s\n",
            strerror(errno));
  }
  /***************setting logtask_msg_que for IPC data Request******************/
  mqd_t messageq_ipc;
  int msgprio_ipc = 20;
  int IPCnum_bytes;
  char IPCmessage[BUFFER_SIZE];

  struct mq_attr IPCmsgq_attr = {.mq_maxmsg = MESSAGEQ_SIZE, // max # msg in queue
                                 .mq_msgsize =
                                     BUFFER_SIZE, // max size of msg in bytes
                                 .mq_flags = 0};

  messageq_ipc =
      mq_open(TEMPERATURE_MSGQ_IPC,      // name
              O_CREAT | O_RDWR, // flags. create a new if dosent already exist
              S_IRWXU,          // mode-read,write and execute permission
              &IPCmsgq_attr);   // attribute
  if (messageq_ipc < 0) {
    initialize = 0;
    sprintf(&(initialize_msg[4][0]), "Temptask-mq_open-IPCQ %s\n",
            strerror(errno));
  } else {
    sprintf(&(initialize_msg[4][0]), "Temptask-mq_open-IPCQ %s\n",
            strerror(errno));
  }
  // set up the signal to request data
  struct sigaction sigactn;
  sigemptyset(&sigactn.sa_mask);
  sigactn.sa_handler = TemptIPChandler;
  ret = sigaction(TEMPSIGNAL_PACKET, &sigactn, NULL);

  if (ret == -1) {
    initialize = 0;
    sprintf(&(initialize_msg[5][0]), "Temp Task sigaction %s\n", strerror(errno));
  } else {
    sprintf(&(initialize_msg[5][0]), "Temp task sigaction %s\n", strerror(errno));
  }

/************Initialize temperatue sensor******************/
#ifdef BBB
  int temperature = initializeTemp();
  char temp_data[2], data_cel_str[BUFFER_SIZE - 200];
  float data_cel;

  if (temperature == -1) {
    initialize = 0;
    sprintf(&(initialize_msg[6][0]), "Temp Sensor init Error\n");
  } else {
    sprintf(&(initialize_msg[6][0]), "Temp Sensor init Success\n");
  }
#endif

  /*****************Mask SIGNALS********************/
  sigset_t mask_bit; // set of signals
  sigemptyset(&mask_bit);
  sigaddset(&mask_bit, LIGHT_SIGNAL_OPT);
  sigaddset(&mask_bit, LIGHT_SIG_HEARTBEAT);
  sigaddset(&mask_bit, LOGGER_SIG_HEARTBEAT);
  sigaddset(&mask_bit, TEMPERATURE_SIG_HEARTBEAT);
  sigaddset(&mask_bit, LOGGER_SIG);
  sigaddset(&mask_bit, SIGCONT);
  sigaddset(&mask_bit, SOCKET_SIG_HEARTBEAT);

  ret =
      pthread_sigmask(SIG_SETMASK, // block the signals in the set argument
                      &mask_bit,       // set argument has list of blocked signals
                      NULL); // if non NULL prev val of signal mask_bit stored here
  if (ret == -1) {
    initialize = 0;
    sprintf(&(initialize_msg[7][0]), "Temp task-pthread_sigmask %s\n",
            strerror(errno));
  } else {
    sprintf(&(initialize_msg[7][0]), "Temp task-pthread_sigmask %s\n",
            strerror(errno));
  }

  // send initialization status
  alert(&initialize_msg[0][0], alertmsg_queue, logtask_msg_que, init);
  alert(&initialize_msg[1][0], alertmsg_queue, logtask_msg_que, init);
  alert(&initialize_msg[2][0], alertmsg_queue, logtask_msg_que, init);
  alert(&initialize_msg[3][0], alertmsg_queue, logtask_msg_que, init);
  alert(&initialize_msg[4][0], alertmsg_queue, logtask_msg_que, init);
#ifdef BBB
  alert(&initialize_msg[5][0], alertmsg_queue, logtask_msg_que, init);
#endif
  alert(&initialize_msg[6][0], alertmsg_queue, logtask_msg_que, init);
  alert(&initialize_msg[7][0], alertmsg_queue, logtask_msg_que, init);

  if (initialize == 0) {
    alert("##All elements not initialized in Temp Task, Not proceeding with "
           "it##\n",
           alertmsg_queue, logtask_msg_que, error);
    while (temperature_close_flag & application_close_flag) {
      sleep(1);
    };
    return NULL;
  }

  else if (initialize == 1)
    alert("##All elements initialized in Temp Task, proceeding with it##\n",
           alertmsg_queue, logtask_msg_que, init);

  /************Creating logpacket*******************/
  logger_pckt temp_log = {.log_level = 1, .log_source = temperatue_Task};
  struct timespec current, expire;

#ifdef BBB
  /*****Looging BBB configurations*******/
  printf("-------------configuring temperature sensor------------ \n\n");
  printf("configuring TLOW register\n");
  char rd_tbyte[2];
  char wr_byte[2];
  
  tlowWrite(temperature);  
  tlowRead(temperature, rd_tbyte);

  float res = temperatureConv(CELCIUS, rd_tbyte);
  printf("TLOW value: %f\n", res);

  thighWrite(temperature);
  thighRead(temperature, rd_tbyte);
  res = temperatureConv(CELCIUS, rd_tbyte);
  printf("THIGH value: %f\n", res);


  configRegRead(temperature, rd_tbyte);
  printf("CONFIG register value %x %x\n", rd_tbyte[0], rd_tbyte[1]);

  wr_byte[0] = rd_tbyte[0] | SHUTDOWN_EN;
  wr_byte[1] = rd_tbyte[1];
  configRegWrite(temperature, wr_byte);

  configRegRead(temperature, rd_tbyte);
  printf("CONFIG register value after SD EN %x %x\n", rd_tbyte[0], rd_tbyte[1]);

  wr_byte[0] = rd_tbyte[0] & SHUTDOWN_DI;
  wr_byte[1] = rd_tbyte[1];
  configRegWrite(temperature, wr_byte);

  configRegRead(temperature, rd_tbyte);
  printf("CONFIG register value after SD DIS_EN %x %x\n", rd_tbyte[0], rd_tbyte[1]);

  wr_byte[0] = rd_tbyte[0];
  wr_byte[1] = rd_tbyte[1] | EMMODE_EN;
  configRegWrite(temperature, wr_byte);

  configRegRead(temperature, rd_tbyte);
  printf("CONFIG register value after setting EMMODE %x %x\n", rd_tbyte[0], rd_tbyte[1]);

  wr_byte[0] = rd_tbyte[0];
  wr_byte[1] = rd_tbyte[1] & EMMODE_DI;
  configRegWrite(temperature, wr_byte);

  configRegRead(temperature, rd_tbyte);
  printf("CONFIG register value after clearing EMMODE %x %x\n", rd_tbyte[0], rd_tbyte[1]);

  wr_byte[0] = rd_tbyte[0];
  wr_byte[1] = rd_tbyte[1] | CONVRATE3;
  configRegWrite(temperature, wr_byte);

  configRegRead(temperature, rd_tbyte);
  printf("CONFIG register value after setting CONVERSION RATE 8Hz %x %x\n", rd_tbyte[0], rd_tbyte[1]);

  wr_byte[0] = rd_tbyte[0] | TM_EN;
  wr_byte[1] = rd_tbyte[1];
  configRegWrite(temperature, wr_byte);

  configRegRead(temperature, rd_tbyte);
  printf("CONFIG register value after setting INTERRUPT MODE %x %x\n", rd_tbyte[0], rd_tbyte[1]);

 printf("-------------complete------------ \n\n");
#endif

  /****************Do this periodically*******************************/
  while (temperature_close_flag & application_close_flag) {

    INTR_LED_OFF;
    // wait for next second
    pthread_mutex_lock(&gtemp_mutex);
    while (gtemp_flag == 0) {
      pthread_cond_wait(&gtemp_condition, &gtemp_mutex);
    }

    pthread_mutex_unlock(&gtemp_mutex);
    gtemp_flag = 0;
    // send HB
    pthread_kill(pthread_info->main, TEMPERATURE_SIG_HEARTBEAT);
// collect temperatue
#ifdef BBB
    temperatureRead(temperature, temp_data);
    data_cel = temperatureConv(temp_format, temp_data);
    printf("temp: %f\n", data_cel);

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

    clock_gettime(CLOCK_MONOTONIC, &current);
    expire.tv_sec = current.tv_sec + 2;
    expire.tv_nsec = current.tv_nsec;
    num_bytes = mq_timedsend(logtask_msg_que, (const char *)&temp_log,
                             sizeof(logger_pckt), msg_priority, &expire);
    if (num_bytes < 0) {
      alert("mq_send to Log Q in TemperatureTask", alertmsg_queue, logtask_msg_que, error);
    }
    /******Log data on IPC Que if requested******/

    if (temp_IPC_flag == 1) {
      temp_IPC_flag = 0;
      // set up time for timed send
      clock_gettime(CLOCK_MONOTONIC, &current);
      expire.tv_sec = current.tv_sec + 2;
      expire.tv_nsec = current.tv_nsec;
      num_bytes = mq_timedsend(messageq_ipc, (const char *)&temp_log,
                               sizeof(logger_pckt), msgprio_ipc, &expire);
      if (num_bytes < 0) {
        alert("mq_send-IPC Q-TemperatureTask Error", alertmsg_queue, logtask_msg_que, error);
      }
    }
    // printf("hi\n");
    //  alert("Test Error",alertmsg_queue,logtask_msg_que,error);
  }
  printf("exiting Temp task\n");
  mq_close(logtask_msg_que);
  mq_close(alertmsg_queue);
  mq_close(messageq_ipc);

  return NULL;
}
