/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 01/30/2018
* DESCRITPTION	: This program consists of a set of cron tasks
		  that will be scheduled to run every 10 minutes.
		  
* SOURCE FILES	: my_crontasks.c
****************************************************************/
#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>     
#include <time.h>

#define BUFFER_DEPTH 256
struct timeval tv;
struct tm* ptm;
