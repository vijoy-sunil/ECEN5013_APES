#include "adps9301.h"
#include "tmp102.h"
#include "includes.h"
#include "messageQue.h"
#include "errorhandling.h"
#include "notify.h"
#include "threads.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

void *socketTask(void *pthread_inf) {

  uint8_t init_state = 1;
  char init_message[7][sizeof(alert_message)];

  int ret;
  threadInfo *ppthread_info = (threadInfo *)pthread_inf;
  /*******Initialize Notification  Message Que*****************/
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
  sprintf(&(init_message[0][0]), "SocketTask-mq_open-notify mq %s\n",
          strerror(errno));
  if (alertmsg_queue < 0)
    init_state = 0;

  /*******Initialize Logger Message Que*****************/
  mqd_t logger_msgq;
  int msg_prio = MSG_PRIO;
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
  sprintf(&(init_message[1][0]), "SocketTask-mq_open-logger mq %s\n",
          strerror(errno));
  if (logger_msgq < 0)
    init_state = 0;

  /*************Sockets*****************************/
  // user defined data structures for data read and write
  sock_req *request = (sock_req *)malloc(sizeof(sock_req));
  if (request == NULL) {
    printf("malloc Error: %s\n", strerror(errno));
    return NULL;
  }

  log_pack *response = (log_pack *)malloc(sizeof(log_pack));
  if (response == NULL) {
    printf("malloc Error: %s\n", strerror(errno));
    return NULL;
  }

  int sockfd;                     // listening FD
  int newsockfd;                  // Client connected FD
  int num_char;                   // No. of characters red/written
  struct sockaddr_in server_addr; // structure containing internet addresss.

  int opt = 1;
  /****Create a new socket*******/
  sockfd = socket(AF_INET,                     // com domain - IPv4
                  SOCK_STREAM | SOCK_NONBLOCK, // com type - TCP
                  0);                          // protocol
  sprintf(&(init_message[2][0]), "SocketTask-socket %s\n", strerror(errno));
  if (sockfd < 0)
    init_state = 0;

  /*****set options for the socket***********/
  ret = setsockopt(sockfd,
                   SOL_SOCKET, // Socket Level Protocol
                   SO_REUSEADDR | SO_REUSEPORT,
                   &opt, // option is enabled
                   sizeof(opt));
  sprintf(&(init_message[3][0]), "SocketTask-setsockopt %s\n", strerror(errno));
  if (ret < 0)
    init_state = 0;
  /***initialize the address structure and bind socket ****/
  bzero((char *)&server_addr, sizeof(server_addr)); // sets all val to 0
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);
  ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  sprintf(&(init_message[4][0]), "SocketTask-bind %s\n", strerror(errno));
  if (ret < 0)
    init_state = 0;

  /**listen on socket for connections**/
  ret = listen(sockfd, 5);
  sprintf(&(init_message[6][0]), "SocketTask-listen %s\n", strerror(errno));
  if (ret < 0)
    init_state = 0;

  /*****************Mask SIGNALS********************/
  sigset_t mask; // set of signals
  sigemptyset(&mask);
  sigaddset(&mask, SIGTEMP);
  sigaddset(&mask, TEMPERATURE_SIG_HEARTBEAT);
  sigaddset(&mask, LIGHT_SIG_HEARTBEAT);
  sigaddset(&mask, SIGLIGHT);
  sigaddset(&mask, SIGLOG_HB);
  sigaddset(&mask, SIGLOG);
  sigaddset(&mask, SIGTEMP_IPC);
  sigaddset(&mask, SIGLIGHT_IPC);

  ret =
      pthread_sigmask(SIG_SETMASK, // block the signals in the set argument
                      &mask,       // set argument has list of blocked signals
                      NULL); // if non NULL prev val of signal mask stored here
  sprintf(&(init_message[5][0]), "SocketTask-pthread_sigmask %s\n",
          strerror(errno));
  if (ret < 0)
    init_state = 0;

  notify(&init_message[0][0], alertmsg_queue, logger_msgq, init);
  notify(&init_message[1][0], alertmsg_queue, logger_msgq, init);
  notify(&init_message[2][0], alertmsg_queue, logger_msgq, init);
  notify(&init_message[3][0], alertmsg_queue, logger_msgq, init);
  notify(&init_message[4][0], alertmsg_queue, logger_msgq, init);
  notify(&init_message[5][0], alertmsg_queue, logger_msgq, init);
  notify(&init_message[6][0], alertmsg_queue, logger_msgq, init);

  if (init_state == 0) {
    notify("##All elements not initialized in Socket Task, Not proceeding with "
           "it##\n",
           alertmsg_queue, logger_msgq, error);
    while (socket_close_flag & application_close_flag) {
      sleep(1);
    };
    free(request);
    free(response);
    return NULL;
  }

  else if (init_state == 1)
    notify("##All elements initialized in Socket Task, proceeding with it##\n",
           alertmsg_queue, logger_msgq, init);

#ifdef BBB
  int temp_handle = initializeTemp(); // Get the Handler
  char temp_data[2], data_cel_str[BUFFER_SIZE - 200];
  float data_cel;

  int light_handle = initializeLight(); // Get the handler
  char lightbuffer[1];
  commandReg(light_handle, CONTROL, WRITE);
  controlReg(light_handle, WRITE, ENABLE, lightbuffer);
  float lumen;
  char data_lumen_str[BUFFER_SIZE - 200];
  uint16_t ch0, ch1;

#endif

  /****block until the client connects to the server and gets its address*****/
  struct sockaddr_in client_addr;
  socklen_t addrlen = sizeof(client_addr); // size of address of client

  // keep doing this repeatedly
  while (socket_close_flag & application_close_flag) {
    pthread_kill(ppthread_info->main, SIGSOCKET_HB); // send HB

    while (1) {
      if ((socket_close_flag & application_close_flag) == 0)
        break;

      newsockfd = accept(sockfd, (struct sockaddr *)&client_addr,
                         (socklen_t *)&addrlen);
      if ((newsockfd > 0))
        break;
      else
        sleep(1);
      // send HB to main
      pthread_kill(ppthread_info->main, SIGSOCKET_HB); // send HB
    }
    if ((socket_close_flag & application_close_flag) == 0)
      break;

    /*****beyond this, execution happens only after client is connected******/
    // prepopulate static elements of response packet
    response->log_source = RemoteRequestSocket_Task;
    response->log_level = 3;

    /****read from the client and write to it*******/

    bzero(request, sizeof(sock_req));

    num_char = read(newsockfd, (char *)request, sizeof(sock_req));
    if (num_char < 0) {
      notify("Socket Task read error", alertmsg_queue, logger_msgq, error);
      break;
    }

    // find the sensor requested to to probe and probe the sensor
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strcpy(response->time_stamp, asctime(tm));

// collect data and populate the log packet
#ifdef BBB
    if (request->sensor == temp) {

      temperatureRead(temp_handle, temp_data);
      if (request->tunit == CELCIUS) {
        data_cel = temperatureConv(CELCIUS, temp_data);
      }
      if (request->tunit == FARENHEIT) {
        data_cel = temperatureConv(FARENHEIT, temp_data);
      }
      if (request->tunit == KELVIN) {
        data_cel = temperatureConv(KELVIN, temp_data);
      }
      sprintf(data_cel_str, "temperature %f", data_cel);
      strcpy(response->log_msg, data_cel_str);
    }
    if (request->sensor == light) {
      if (request->lunit == LUMEN) {
        ch0 = adcDataRead(light_handle, 0);
        ch1 = adcDataRead(light_handle, 1);
        lumen = reportLumen(ch0, ch1);
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

#else
    if (request->sensor == temp) {
      strcpy(response->log_msg, "TEMP");
    }
    if (request->sensor == light) {
      strcpy(response->log_msg, "LIGHT");
    }
#endif
    // send the read data
    num_char = write(newsockfd, response, sizeof(log_pack));
    if (num_char < 0) {
      notify("Socket Task write error", alertmsg_queue, logger_msgq, error);
      break;
    }

    //                sleep(2);
  }
  printf("Exiting Socket Task\n");
  free(request);
  free(response);

  return NULL;
}
