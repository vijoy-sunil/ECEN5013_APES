/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 01/30/2018
* DESCRITPTION	: This program does a system call that copies an 
		  array from user to kernel space, sorts it and
		  copies it back to user space
		  
* SOURCE FILES	: call_syscall_sorter.c
****************************************************************/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>     
#include <time.h>

#define BUFFER_DEPTH 256
#define COUNT 5

