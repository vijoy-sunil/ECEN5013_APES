#include "TemperatureTask.h"


sig_atomic_t IPC_temperature_flag;


void *TemperatureTask(void *pthread_inf) {

  uint8_t initialize = 1;
  char initialize_msg[8][sizeof(alert_message)];
  IPC_temperature_flag = 0;
  int ret;
  threadTaskAttr *pthread_info = (threadTaskAttr *)pthread_inf;

  mqd_t alertmsg_queue;
  int msg_prio_err = ERROR_MESSAGE_PRIORITY;
  int error_no;
  struct mq_attr msgq_attr_err = {.mq_maxmsg = MESSAGEQ_SIZE,.mq_msgsize = BUFFER_SIZE,.mq_flags = 0};

  alertmsg_queue =mq_open(ALERT_MSGQ_PCKT, O_CREAT | O_RDWR, S_IRWXU, &msgq_attr_err);

  if (alertmsg_queue < 0) 
  {
    initialize = 0;
    sprintf(&(initialize_msg[0][0]), "Failure temp task msgq  %s\n",strerror(errno));
  } 
  else 
  {
    sprintf(&(initialize_msg[0][0]), "Success temp task msgq %s\n",strerror(errno));
  }

  ret = Temp_init_timer(); 

  if (ret == -1) 
  {
    initialize = 0;
    sprintf(&(initialize_msg[1][0]), "Failure Temptask Temp_init_timer \n");
  } 
  else 
  {
    sprintf(&(initialize_msg[1][0]), "Success Temptask Temp_init_timer \n");
  }

  ret = pthread_mutex_init(&lock_temp, NULL);

  if (ret == -1) 
  {
    initialize = 0;
    sprintf(&(initialize_msg[2][0]), "failure temp mutex %s\n",strerror(errno));
  } 
  else 
  {
    sprintf(&(initialize_msg[2][0]), "success temp mutex:%s\n",strerror(errno));
  }

  mqd_t logtask_msg_que;
  int msg_priority = 30;
  int num_bytes;
  char message[BUFFER_SIZE];
  struct mq_attr msgq_attr = {.mq_maxmsg = MESSAGEQ_SIZE,.mq_msgsize = BUFFER_SIZE, .mq_flags = 0};

  logtask_msg_que = mq_open(LOGGER_MSGQ_IPC,  O_CREAT | O_RDWR,  S_IRWXU,  &msgq_attr); 

  if (logtask_msg_que < 0) 
  {
    initialize = 0;
    sprintf(&(initialize_msg[3][0]), "Failure temp loggerq %s\n",strerror(errno));
  } 
  else 
  {
    sprintf(&(initialize_msg[3][0]), "Success temp loggerq %s\n",strerror(errno));
  }

  mqd_t messageq_ipc;
  int msgprio_ipc = 20;
  int IPCnum_bytes;
  char IPCmessage[BUFFER_SIZE];

  struct mq_attr IPCmsgq_attr = {.mq_maxmsg = MESSAGEQ_SIZE,.mq_msgsize = BUFFER_SIZE,.mq_flags = 0};

  messageq_ipc =mq_open(TEMPERATURE_MSGQ_IPC, O_CREAT | O_RDWR,S_IRWXU,  &IPCmsgq_attr); 

  if (messageq_ipc < 0) 
  {
    initialize = 0;
    sprintf(&(initialize_msg[4][0]), "Failure temptask msgqipc%s\n", strerror(errno));
  } 
  else 
  {
    sprintf(&(initialize_msg[4][0]), "Success temptask msgqipc %s\n",strerror(errno));
  }


  struct sigaction sigactn;
  sigemptyset(&sigactn.sa_mask);
  sigactn.sa_handler = IPCTemperature_Handl;
  ret = sigaction(TEMPSIGNAL_PACKET, &sigactn, NULL);

  if (ret == -1) 
  {
    initialize = 0;
    sprintf(&(initialize_msg[5][0]), "Failure Temp sigaction %s\n", strerror(errno));
  } else {
    sprintf(&(initialize_msg[5][0]), "Success Temp sigaction %s\n", strerror(errno));
  }

  int temperature = Temp_sensor_init();
  char temp_data[2], data_cel_str[BUFFER_SIZE - 200];
  float data_cel;

  if (temperature == -1) 
  {
    initialize = 0;
    sprintf(&(initialize_msg[6][0]), "Temp init Error\n");
  } 
  else 
  {
    sprintf(&(initialize_msg[6][0]), "Temp init Success\n");
  }

  sigset_t mask_bit; 
  sigemptyset(&mask_bit);
  sigaddset(&mask_bit, LIGHT_SIGNAL_OPT);
  sigaddset(&mask_bit, LIGHT_SIG_HEARTBEAT);
  sigaddset(&mask_bit, LOGGER_SIG_HEARTBEAT);
  sigaddset(&mask_bit, TEMPERATURE_SIG_HEARTBEAT);
  sigaddset(&mask_bit, LOGGER_SIG);
  sigaddset(&mask_bit, SIGCONT);
  sigaddset(&mask_bit, SOCKET_SIG_HEARTBEAT);

  ret = pthread_sigmask(SIG_SETMASK,&mask_bit, NULL); 
  
  if (ret == -1) 
  {
    initialize = 0;
    sprintf(&(initialize_msg[7][0]), "Temp task-pthread_sigmask %s\n",strerror(errno));
  } 
  else 
  {
    sprintf(&(initialize_msg[7][0]), "Temp task-pthread_sigmask %s\n",strerror(errno));
  }


  alert(&initialize_msg[0][0], alertmsg_queue, logtask_msg_que, init);
  alert(&initialize_msg[1][0], alertmsg_queue, logtask_msg_que, init);
  alert(&initialize_msg[2][0], alertmsg_queue, logtask_msg_que, init);
  alert(&initialize_msg[3][0], alertmsg_queue, logtask_msg_que, init);
  alert(&initialize_msg[4][0], alertmsg_queue, logtask_msg_que, init);
  alert(&initialize_msg[5][0], alertmsg_queue, logtask_msg_que, init);
  alert(&initialize_msg[6][0], alertmsg_queue, logtask_msg_que, init);
  alert(&initialize_msg[7][0], alertmsg_queue, logtask_msg_que, init);

  if (initialize == 0) 
  {
    alert("TEMP TASK INIT FAILURE\n",alertmsg_queue, logtask_msg_que, error);

    while (temperature_close_flag & application_close_flag) 
    {
      sleep(1);
    };
    return NULL;
  }

  else if (initialize == 1)
    alert("TEMP TASK INIT SUCCESS\n",alertmsg_queue, logtask_msg_que, init);


  logger_pckt temperature_log = {.log_level = 1, .log_source = temperatue_Task};

  struct timespec current, expire;

  printf("TLOW register\n");
  char rd_tbyte[2];
  char wr_byte[2];
  
  temp_TLOW_write(temperature);  
  tlowRead(temperature, rd_tbyte);

  float res = covert_temperature(CELCIUS, rd_tbyte);
  printf("TLOW value: %f\n", res);

  temp_THIGH_write(temperature);
  temp_THIGH_read(temperature, rd_tbyte);
  res = covert_temperature(CELCIUS, rd_tbyte);
  printf("THIGH value: %f\n", res);


  temp_CONFIG_read(temperature, rd_tbyte);
  printf("config default %x %x\n", rd_tbyte[0], rd_tbyte[1]);

  wr_byte[0] = rd_tbyte[0] | SHUTDOWN_EN;
  wr_byte[1] = rd_tbyte[1];
  temp_CONFIG_write(temperature, wr_byte);

  temp_CONFIG_read(temperature, rd_tbyte);
  printf("shutdown enable %x %x\n", rd_tbyte[0], rd_tbyte[1]);

  wr_byte[0] = rd_tbyte[0] & SHUTDOWN_DI;
  wr_byte[1] = rd_tbyte[1];
  temp_CONFIG_write(temperature, wr_byte);

  temp_CONFIG_read(temperature, rd_tbyte);
  printf("Shutdown disable %x %x\n", rd_tbyte[0], rd_tbyte[1]);

  wr_byte[0] = rd_tbyte[0];
  wr_byte[1] = rd_tbyte[1] | EMMODE_EN;
  temp_CONFIG_write(temperature, wr_byte);

  temp_CONFIG_read(temperature, rd_tbyte);
  printf("Set EM mode %x %x\n", rd_tbyte[0], rd_tbyte[1]);

  wr_byte[0] = rd_tbyte[0];
  wr_byte[1] = rd_tbyte[1] | CONVRATE3;
  temp_CONFIG_write(temperature, wr_byte);

  temp_CONFIG_read(temperature, rd_tbyte);
  printf("Conv rate 8Hz %x %x\n", rd_tbyte[0], rd_tbyte[1]);

  wr_byte[0] = rd_tbyte[0] | TM_EN;
  wr_byte[1] = rd_tbyte[1];
  temp_CONFIG_write(temperature, wr_byte);

  while (temperature_close_flag & application_close_flag) 
  {
    INTR_LED_OFF;

    pthread_mutex_lock(&lock_temp);
    while (temp_flag_glb == 0) 
    {
      pthread_cond_wait(&cond_var_temp, &lock_temp);
    }

    pthread_mutex_unlock(&lock_temp);

    temp_flag_glb = 0;
    pthread_kill(pthread_info->main, TEMPERATURE_SIG_HEARTBEAT);

    temperatureRead(temperature, temp_data);
    data_cel = covert_temperature(temp_format, temp_data);
    printf("TEMPERATURE: %f\n", data_cel);

    sprintf(data_cel_str, "TEMPERATURE %f", data_cel);
    strcpy(temperature_log.log_msg, data_cel_str);


    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strcpy(temperature_log.time_stamp, asctime(tm));


    clock_gettime(CLOCK_MONOTONIC, &current);
    expire.tv_sec = current.tv_sec + 2;
    expire.tv_nsec = current.tv_nsec;
    num_bytes = mq_timedsend(logtask_msg_que, (const char *)&temperature_log,sizeof(logger_pckt), msg_priority, &expire);

    if (num_bytes < 0) 
    {
      alert("mq_send to Log Q in TemperatureTask", alertmsg_queue, logtask_msg_que, error);
    }

    if (IPC_temperature_flag == 1) 
    {
      IPC_temperature_flag = 0;
      clock_gettime(CLOCK_MONOTONIC, &current);
      expire.tv_sec = current.tv_sec + 2;
      expire.tv_nsec = current.tv_nsec;
      num_bytes = mq_timedsend(messageq_ipc, (const char *)&temperature_log,sizeof(logger_pckt), msgprio_ipc, &expire);

      if (num_bytes < 0) 
      {
        alert("mq_send-IPC Q-TemperatureTask Error", alertmsg_queue, logtask_msg_que, error);
      }
    }
  }
  printf("TEMP TASK QUITTING\n");
  mq_close(logtask_msg_que);
  mq_close(alertmsg_queue);
  mq_close(messageq_ipc);

  return NULL;
}


void IPCTemperature_Handl(int sig) {
  if (sig == TEMPSIGNAL_PACKET) {
    printf("Caught signal IPCTemperature_Handl\n");
    IPC_temperature_flag = 1;
  }
}
