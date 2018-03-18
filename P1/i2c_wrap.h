#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int i2cInit(char *dev_path, int i2c_file_handler, int slave_addr);
int i2cRead(int i2c_file_handler, char *buffer, int num_bytes);
int i2cWrite(int i2c_file_handler, char *buffer, int num_bytes);
