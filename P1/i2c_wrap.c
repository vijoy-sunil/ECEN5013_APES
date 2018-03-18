
#include "includes.h"
#include "i2cWrappper.h"
#include <pthread.h>

int i2cInit(char *dev_path, int i2c_file_handler, int slave_addr) {
  int rc, ret;
  char test_buf[1];
  pthread_mutex_lock(&lock_i2c);

  i2c_file_handler = open(dev_path, O_RDWR);

  if (i2c_file_handler < 0) {
    err(errno, "Tried to open '%s'", dev_path);
    ret = -1;
  } else
    ret = i2c_file_handler;

  rc = ioctl(i2c_file_handler, I2C_SLAVE, slave_addr);
  if (rc < 0) {
    err(errno, "Tried to set device address '0x%02x'", slave_addr);
    ret = -1;
  }
  if (read(i2c_file_handler, test_buf, 1) != 1) {
    ret = -1;
  }
  pthread_mutex_unlock(&lock_i2c);
  return ret;
}

int i2cRead(int i2c_file_handler, char *buffer, int num_bytes) {
  int ret;
  pthread_mutex_lock(&lock_i2c);
  if (read(i2c_file_handler, buffer, num_bytes) != num_bytes)
    ret = -1;
  else
    ret = 0;
  pthread_mutex_unlock(&lock_i2c);
  return ret;
}

int i2cWrite(int i2c_file_handler, char *buffer, int num_bytes) {
  int ret;
  pthread_mutex_lock(&lock_i2c);
  if (write(i2c_file_handler, buffer, num_bytes) != num_bytes)
    ret = -1;
  else
    ret = 0;
  pthread_mutex_unlock(&lock_i2c);
  return ret;
}
