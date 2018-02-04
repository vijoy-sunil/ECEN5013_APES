/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 01/30/2018
* DESCRITPTION	: This is a kernel module that user a kernel timer
		  (500 ms) to call a function periodically that
		  writes to the kernel log buffer,
		 
* SOURCE FILES	: timer_module.c
****************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("TIMER MODULE");
MODULE_AUTHOR("VIJOY SUNIL KUMAR");

static struct timer_list ktimer;
