#include "LoggerTask.h"

sig_atomic_t logger_data_flag;

void *logTask(void *pthread_inf) {

  int ret;
  uint8_t initialize = 1;
  char initialize_msg[4][sizeof(alert_message)];

  threadTaskAttr *pthread_info = (threadTaskAttr *)pthread_inf;

  mqd_t alertmsg_queue;
  int msg_prio_err = ERROR_MESSAGE_PRIORITY;
  int error_no;
  struct mq_attr msgq_attr_err = {.mq_maxmsg = MESSAGEQ_SIZE,.mq_msgsize =BUFFER_SIZE, .mq_flags = 0};

  alertmsg_queue =mq_open(ALERT_MSGQ_PCKT, O_CREAT | O_RDWR, S_IRWXU,&msgq_attr_err);

  if (alertmsg_queue < 0) 
  {
    initialize = 0;
    sprintf(&(initialize_msg[1][0]), "Failure Log task msgq %s\n",strerror(errno));
  } 

  else 
  {
    sprintf(&(initialize_msg[1][0]), "Success Log task msgq %s\n",strerror(errno));
  }

  mqd_t logtask_msg_que;
  int msg_priority;
  int num_bytes;

  logger_pckt *log = (logger_pckt *)malloc(sizeof(logger_pckt));

  if (log == NULL) 
  {
    perror("Log Task malloc Error");
    return NULL;
  }

  struct mq_attr msgq_attr = {.mq_maxmsg = MESSAGEQ_SIZE,.mq_msgsize =BUFFER_SIZE,.mq_flags = 0};

  logtask_msg_que =mq_open(LOGGER_MSGQ_IPC,O_CREAT | O_RDWR, S_IRWXU,&msgq_attr); 

  if (logtask_msg_que < 0) 
  {
    initialize = 0;
    sprintf(&(initialize_msg[2][0]), "Failure logger open %s\n",strerror(errno));
  } 
  else 
  {
    sprintf(&(initialize_msg[2][0]), "Success logger open %s\n",strerror(errno));
  }

  FILE *pfd = fopen(fileid, "w");
  if (pfd == NULL) 
  {
    initialize = 0;
    sprintf(&(initialize_msg[3][0]), "Log Task fopen %s\n", strerror(errno));
  } 
  else 
  {

    sprintf(&(initialize_msg[3][0]), "Log Task fopen %s\n", strerror(errno));
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

  ret =pthread_sigmask(SIG_SETMASK,&mask_bit, NULL); 

  if (ret == -1) 
  {
    initialize = 0;
    sprintf(&(initialize_msg[0][0]), "Failure LogTask Sigmask %s\n", strerror(errno));
  } 
  else 
  {
    sprintf(&(initialize_msg[0][0]), "Success LogTask Sigmask:%s\n", strerror(errno));
  }

#ifdef PRINT_IPC_MSGQ

  mqd_t IPCmsgqT, IPCmsgqL;
  int IPCmsg_prio = 20;
  int IPCnum_bytes;
  char IPCmessage[BUFFER_SIZE];

  struct mq_attr IPCmsgq_attr = {.mq_maxmsg = MESSAGEQ_SIZE,.mq_msgsize =BUFFER_SIZE,.mq_flags = 0};

  IPCmsgqT = mq_open(TEMPERATURE_MSGQ_IPC,O_CREAT | O_RDWR, S_IRWXU,&IPCmsgq_attr);

  if (IPCmsgqT < 0) 
  {
    perror("mq_open-logTask Error:");
    return NULL;
  } 
  else
    printf("IPC temperature Messgae Que Opened in logTask\n");

  IPCmsgqL =
      mq_open(LIGHT_MSGQ_IPC,     // name
              O_CREAT | O_RDWR, // flags. create a new if dosent already exist
              S_IRWXU,          // mode-read,write and execute permission
              &IPCmsgq_attr);   // attribute
  if (IPCmsgqL < 0) {
    perror("mq_open-logTask Error:");
    return NULL;
  } else
    printf("IPC light Messgae Que Opened in logTask\n");
#endif
  struct timespec current, expire;

  // send initialization message
  alert(&initialize_msg[0][0], alertmsg_queue, -1, init);
  alert(&initialize_msg[1][0], alertmsg_queue, -1, init);
  alert(&initialize_msg[2][0], alertmsg_queue, -1, init);
  alert(&initialize_msg[3][0], alertmsg_queue, -1, init);

  if (initialize == 0) {
    alert("##All elements not initialized in Log Task. Not proceeding with "
           "it##\n",
           alertmsg_queue, logtask_msg_que, error);
    while (logger_close_flag & application_close_flag) {
      sleep(1);
    };
    return NULL;
  }

  else if (initialize == 1)
    alert("##All elements initialized in Log Task, proceeding with it##\n",
           alertmsg_queue, logtask_msg_que, init);

  /*******************Do this in LOOP************************************/
  while (logger_close_flag & application_close_flag) {

    pthread_kill(pthread_info->main, LOGGER_SIG_HEARTBEAT); // send HB
    // empty the message que
    do {
      // read from queue
      clock_gettime(CLOCK_MONOTONIC, &current);
      expire.tv_sec = current.tv_sec + 5;
      expire.tv_nsec = current.tv_nsec;

      num_bytes = mq_timedreceive(logtask_msg_que, (char *)log, BUFFER_SIZE, &msg_priority,
                                  &expire);

      // write to a log file
      if (num_bytes > 0) {
        fprintf(pfd, "%s  %d  %d  %s\n\n", ((logger_pckt *)log)->time_stamp,
                ((logger_pckt *)log)->log_level, ((logger_pckt *)log)->log_source,
                ((logger_pckt *)log)->log_msg);
        fflush(pfd);
      }
    } while (num_bytes > 0);
// reregister after emptying the que
// ret  = mq_notify(logtask_msg_que,&sig_ev);
// if(ret == -1) {perror("mq_notify-main"); return NULL;}

/*&&&&&&&&&&& printing data for IPC message Q test&&&&&&&&&&&&&&*/

#ifdef PRINT_IPC_MSGQ
    clock_gettime(CLOCK_MONOTONIC, &current);
    expire.tv_sec = current.tv_sec + 1;
    expire.tv_nsec = current.tv_nsec;
    num_bytes =
        mq_timedreceive(IPCmsgqT, (char *)log, BUFFER_SIZE, &IPCmsg_prio, &expire);
    if (num_bytes > 0) {
      printf("data read on IPC msg Q:%s\n", log->time_stamp);
    }

    clock_gettime(CLOCK_MONOTONIC, &current);
    expire.tv_sec = current.tv_sec + 1;
    expire.tv_nsec = current.tv_nsec;
    num_bytes =
        mq_timedreceive(IPCmsgqL, (char *)log, BUFFER_SIZE, &IPCmsg_prio, &expire);
    if (num_bytes > 0) {
      printf("data read on IPC msg Q:%s\n", log->time_stamp);
    }
#endif
    sleep(1);
  }

  printf("exiting Log task\n");
  fclose(pfd);
  mq_close(logtask_msg_que);
  mq_close(alertmsg_queue);
#ifdef PRINT_IPC_MSGQ
  mq_close(IPCmsgqT);
  mq_close(IPCmsgqL);
#endif
  free(log);
  return NULL;
}
