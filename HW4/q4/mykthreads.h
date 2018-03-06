/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 03/02/2018
* DESCRITPTION	: This program creates a kernel module that allows two threads
		  to communicate via queues (kfifo). The first thread sends
		  information to the second thread on a timed interval through the fifo.
		  The second thread should take data from the kfifo and print it to the 
		  kernel logger. The info comprises of PID and vruntime of the previous, 
		  current, and next PID  
		 
* SOURCE FILES	: mykthreads.c
****************************************************************/

//Reference: https://stuff.mit.edu/afs/sipb/contrib/linux/samples/kfifo/record-example.c

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/delay.h>
#include <linux/kfifo.h>
#include <linux/kthread.h>


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("KTHREADS AND KFIFO MODULE");
MODULE_AUTHOR("VIJOY SUNIL KUMAR");

static struct task_struct *thread1;
static struct task_struct *thread2;
static struct task_struct *fifo_data;

#define FIFO_DEPTH 16
//typedef STRUCT_KFIFO_REC_1(FIFO_DEPTH) my_fifo_t;

static DECLARE_KFIFO(my_fifo, struct task_struct*, FIFO_DEPTH);
//static my_fifo_t my_fifo;

static DEFINE_MUTEX(lock_my_fifo);

unsigned int rt;



