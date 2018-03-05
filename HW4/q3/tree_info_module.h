/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 03/02/2018
* DESCRITPTION	: This is a kernel module that takes a process ID to 
		  investigate the process tree lineage. This module 
		  should print information about the parent processes 
		  as it traverses backwards up the process tree until 
		  it cannot go any further.

 		For each process it goes through, it should print the below 
		metrics on that process:
		○ Thread Name
		○ Process ID
		○ Process Status
		○ Number of children
		○ Nice Value
		 
* SOURCE FILES	: tree_info_module.c
****************************************************************/

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/list.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PROCESS TREE INFO MODULE");
MODULE_AUTHOR("VIJOY SUNIL KUMAR");

struct task_struct *task;
struct pid *pid_struct;

int get_numChild(struct list_head *child);
