/***************************************************************
* AUTHOR  : Vijoy Sunil Kumar
* DATE    : 03/08/2018
* DESCRITPTION  : The remote reuqest socket task
                  
* SOURCE FILES  : client.c
****************************************************************/

#include "../includes.h"
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "client_test.h"

int test_client_data(char *buf, int threadoption) {

  int shmem_fd; /*shared memory file desxriptor*/
  void *pshmem_obj;
  const char name[] = "OS";

  shmem_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
  if (shmem_fd == -1)
    perror("shm_open");
  ftruncate(shmem_fd, TEST_SIZE);

  pshmem_obj =
      mmap(0, BUFFER_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shmem_fd, 0);
  if (pshmem_obj == NULL)
    perror("mmap");

  if (threadoption == 0) { // write
    if (memcpy((void *)pshmem_obj, (const void *)buf, TEST_SIZE) == NULL)
      perror("memcpy");
    printf("written %s\n", buf);
    return 1;
  }

  if (threadoption == 1) { // read
    bzero(buf, TEST_SIZE);
    if (memcpy(buf, pshmem_obj, TEST_SIZE) == NULL)
      perror("memcpy");
    //`` printf("read %s\n", buf);
    shm_unlink(name);
    return 1;
  }
}
