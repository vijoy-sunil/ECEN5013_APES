/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 01/30/2018
* DESCRITPTION	: Header for System call definition that copies 
		  data from user space to kernel space, sorts them 
		  from largest to smallest and copies it back to 
		  user space.
		  
* SOURCE FILES	: syscall_sorter.c
****************************************************************/
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
