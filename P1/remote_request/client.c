/***************************************************************
* AUTHOR  : Vijoy Sunil Kumar
* DATE    : 03/08/2018
* DESCRITPTION  : The remote reuqest socket task
                  
* HEADER FILES  : client_test.h
****************************************************************/

#include "../includes.h"
#include "client_test.h"
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

int gclose;

void SIGNAL_INTERRUPT_HANDL(int sig) {
  if (sig == SIGINT) {
    gclose = 0;
    printf("\ncleared application_close_flag flag\n");
  }
}

int main() {
  signal(SIGINT, SIGNAL_INTERRUPT_HANDL);
  gclose = 1;
  int ret;
  int sockfd;                     // listening FD
  int newsockfd;                  // Client connected FD
  int num_char;                   // No. of characters red/written
  struct sockaddr_in server_addr; // structure containing internet addresss.

  sock_req *request = (sock_req *)malloc(sizeof(sock_req));
  if (request == NULL) {
    printf("malloc Error: %s\n", strerror(errno));
    return -1;
  }

  logger_pckt *response = (logger_pckt *)malloc(sizeof(logger_pckt));
  if (response == NULL) {
    printf("malloc Error: %s\n", strerror(errno));
    return -1;
  }

  char *data = (char *)malloc(sizeof("TEMP"));
  if (response == NULL) {
    printf("malloc Error: %s\n", strerror(errno));
    return -1;
  }

  sockfd = socket(AF_INET,  SOCK_STREAM, 0);   

  if (sockfd < 0) {
    printf("socket Error:%s\n", strerror(errno));
    return -1;
  }

  memset(&server_addr, '0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);


  ret = inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
  if (ret < 0) {
    printf("inet_pton Error:%s\n", strerror(errno));
    return -1;
  }

  ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

  if (ret < 0) {
    printf("connect Error:%s\n", strerror(errno));
    return -1;
  }

  printf("Enter 1 for temperature in F, 2 for temperature in C, 3 for Kelvin. 4 for ""DAY/NIGHT, 5 for lumens");
  int input;
  scanf("%d", &input);

  switch (input) {
  case 1:
    request->sensor = temperature;
    request->tunit = FARENHEIT;
    break;
  case 2:
    request->sensor = temperature;
    request->tunit = CELCIUS;
    break;
  case 3:
    request->sensor = temperature;
    request->tunit = KELVIN;
    break;
  case 4:
    request->sensor = light;
    request->lunit = DAY_NIGHT;
    break;
  case 5:
    request->sensor = light;
    request->lunit = LUMEN;
    break;

  default:
    request->sensor = temperature;
    request->tunit = CELCIUS;
  }

  num_char = send(sockfd, request, sizeof(sock_req), 0);
  printf("CHILD SENT %d bytes\n", num_char);

  num_char = read(sockfd, (char *)response, sizeof(logger_pckt));

#ifndef TEST
  printf("\nRead in Client\n");
  printf("time     :%s\n", ((logger_pckt *)response)->time_stamp);
  printf("source   :%d\n", ((logger_pckt *)response)->log_source);
  printf("message  :%s\n", ((logger_pckt *)response)->log_msg);
  printf("log level:%d\n", ((logger_pckt *)response)->log_level);
#endif

#ifdef TEST

  strcpy(data, (char *)(((logger_pckt *)response)->log_msg));
  printf("Read message  :%s\n", data);
  test_client_data((char *)data, 0);

#endif

  printf("Exiting Client\n");
  free(request);
  free(response);
  free(data);

  return 0;
}
