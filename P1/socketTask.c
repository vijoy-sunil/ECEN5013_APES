#include "socketTask.h"

void *socketTask(void *pthread_inf) {

  uint8_t initialize = 1;
  char initialize_msg[7][sizeof(alert_message)];

  int ret;
  threadTaskAttr *pthread_info = (threadTaskAttr *)pthread_inf;

  mqd_t alertmsg_queue;
  int msg_prio_err = ERROR_MESSAGE_PRIORITY;
  int error_no;
  struct mq_attr msgq_attr_err = {.mq_maxmsg = MESSAGEQ_SIZE,.mq_msgsize =BUFFER_SIZE, .mq_flags = 0};

  alertmsg_queue =mq_open(ALERT_MSGQ_PCKT, O_CREAT | O_RDWR,  S_IRWXU, &msgq_attr_err); 

  sprintf(&(initialize_msg[0][0]), "Socket task msgque %s\n", strerror(errno));

  if (alertmsg_queue < 0)
    initialize = 0;

  mqd_t logtask_msg_que;
  int msg_priority = MESSAGE_PRIORITY;
  int num_bytes;
  char message[BUFFER_SIZE];
  struct mq_attr msgq_attr = {.mq_maxmsg = MESSAGEQ_SIZE,.mq_msgsize = BUFFER_SIZE,.mq_flags = 0};

  logtask_msg_que = mq_open(LOGGER_MSGQ_IPC, O_CREAT | O_RDWR, S_IRWXU, &msgq_attr);

  sprintf(&(initialize_msg[1][0]), "Socket task msgque logger %s\n",strerror(errno));

  if (logtask_msg_que < 0)
    initialize = 0;

  sock_req *request = (sock_req *)malloc(sizeof(sock_req));
  if (request == NULL) 
  {
    printf("malloc Error: %s\n", strerror(errno));
    return NULL;
  }

  logger_pckt *response = (logger_pckt *)malloc(sizeof(logger_pckt));
  if (response == NULL) 
  {
    printf("malloc Error: %s\n", strerror(errno));
    return NULL;
  }

  int sockfd;                     
  int newsockfd;                  
  int num_char;                   
  struct sockaddr_in server_addr; 

  int opt = 1;

  sockfd = socket(AF_INET,SOCK_STREAM | SOCK_NONBLOCK,0); 

  sprintf(&(initialize_msg[2][0]), "Socket %s\n", strerror(errno));

  if (sockfd < 0)
    initialize = 0;


  ret = setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT,&opt,sizeof(opt));

  sprintf(&(initialize_msg[3][0]), "Socket setsockopt %s\n", strerror(errno));
  if (ret < 0)
    initialize = 0;

  bzero((char *)&server_addr, sizeof(server_addr)); // sets all val to 0
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);
  ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

  sprintf(&(initialize_msg[4][0]), "BIND COMPLETE %s\n", strerror(errno));
  if (ret < 0)
    initialize = 0;


  ret = listen(sockfd, 5);
  sprintf(&(initialize_msg[6][0]), "LISTENING %s\n", strerror(errno));
  if (ret < 0)
    initialize = 0;

  sigset_t mask_bit; // set of signals
  sigemptyset(&mask_bit);
  sigaddset(&mask_bit, TEMPERATURE_SIGNAL_OPT);
  sigaddset(&mask_bit, TEMPERATURE_SIG_HEARTBEAT);
  sigaddset(&mask_bit, LIGHT_SIG_HEARTBEAT);
  sigaddset(&mask_bit, LIGHT_SIGNAL_OPT);
  sigaddset(&mask_bit, LOGGER_SIG_HEARTBEAT);
  sigaddset(&mask_bit, LOGGER_SIG);
  sigaddset(&mask_bit, TEMPSIGNAL_PACKET);
  sigaddset(&mask_bit, LIGHTSIGNAL_PACKET);

  ret =pthread_sigmask(SIG_SETMASK, &mask_bit,NULL); 

  sprintf(&(initialize_msg[5][0]), "Socket pthread %s\n", strerror(errno));
  if (ret < 0)
    initialize = 0;

  alert(&initialize_msg[0][0], alertmsg_queue, logtask_msg_que, init);
  alert(&initialize_msg[1][0], alertmsg_queue, logtask_msg_que, init);
  alert(&initialize_msg[2][0], alertmsg_queue, logtask_msg_que, init);
  alert(&initialize_msg[3][0], alertmsg_queue, logtask_msg_que, init);
  alert(&initialize_msg[4][0], alertmsg_queue, logtask_msg_que, init);
  alert(&initialize_msg[5][0], alertmsg_queue, logtask_msg_que, init);
  alert(&initialize_msg[6][0], alertmsg_queue, logtask_msg_que, init);

  if (initialize == 0) {
    alert("SOCKET TASK FAILURE\n",alertmsg_queue, logtask_msg_que, error);

    while (socket_close_flag & application_close_flag) 
    {
      sleep(1);
    };

    free(request);
    free(response);
    return NULL;
  }

  else if (initialize == 1)
    alert("SOCKET TASK SUCCESS\n",alertmsg_queue, logtask_msg_que, init);

  int temp_handle = Temp_sensor_init(); // Get the Handler
  char temp_data[2], data_cel_str[BUFFER_SIZE - 200];
  float data_cel;

  int light_handle = init_light_sensor(); // Get the handler
  char lightbuffer[1];
  commandReg(light_handle, CONTROL, WRITE);
  controlReg(light_handle, WRITE, ENABLE, lightbuffer);
  float lumen;
  char data_lumen_str[BUFFER_SIZE - 200];
  uint16_t ch0, ch1;


  struct sockaddr_in client_addr;
  socklen_t addrlen = sizeof(client_addr); 

  while (socket_close_flag & application_close_flag) 
  {
    pthread_kill(pthread_info->main, SOCKET_SIG_HEARTBEAT); 

    while (1) 
    {
      if ((socket_close_flag & application_close_flag) == 0)
        break;

      newsockfd = accept(sockfd, (struct sockaddr *)&client_addr,
                         (socklen_t *)&addrlen);
      if ((newsockfd > 0))
        break;
      else
        sleep(1);
      pthread_kill(pthread_info->main, SOCKET_SIG_HEARTBEAT); // send HB
    }
    if ((socket_close_flag & application_close_flag) == 0)
      break;

    response->log_source = RemoteRequestSocket_Task;
    response->log_level = 3;


    bzero(request, sizeof(sock_req));

    num_char = read(newsockfd, (char *)request, sizeof(sock_req));
    if (num_char < 0) 
    {
      alert("Socket Task read error", alertmsg_queue, logtask_msg_que, error);
      break;
    }


    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strcpy(response->time_stamp, asctime(tm));

    if (request->sensor == temperature) {

      temperatureRead(temp_handle, temp_data);
      if (request->tunit == CELCIUS) {
        data_cel = covert_temperature(CELCIUS, temp_data);
      }
      if (request->tunit == FARENHEIT) {
        data_cel = covert_temperature(FARENHEIT, temp_data);
      }
      if (request->tunit == KELVIN) {
        data_cel = covert_temperature(KELVIN, temp_data);
      }
      sprintf(data_cel_str, "temperature %f", data_cel);
      strcpy(response->log_msg, data_cel_str);
    }
    if (request->sensor == light) {
      if (request->lunit == LUMEN) {
        ch0 = LightSensorRead(light_handle, 0);
        ch1 = LightSensorRead(light_handle, 1);
        lumen = LumenOut(ch0, ch1);
        sprintf(data_lumen_str, "lumen %f", lumen);
        strcpy(response->log_msg, data_lumen_str);
      }
      if (request->lunit == DAY_NIGHT) {
        int r = reportStatus(light_handle);
        if (r == DAY)
          strcpy(response->log_msg, "DAY");
        else if (r == NIGHT)
          strcpy(response->log_msg, "NIGHT");
      }
    }

    // send the read data
    num_char = write(newsockfd, response, sizeof(logger_pckt));
    if (num_char < 0) {
      alert("Socket Task write error", alertmsg_queue, logtask_msg_que, error);
      break;
    }

  }
  printf("SOCKET TASK QUIT\n");
  free(request);
  free(response);

  return NULL;
}
