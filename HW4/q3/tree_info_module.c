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
		 
* HEADER FILES	: tree_info_module.h
****************************************************************/



#include "tree_info_module.h"

int mypid = -1;
module_param(mypid, int, S_IRUGO);

int get_numChild(struct list_head *child)
{
	int numChild = 0;
	struct list_head *p;
			
								//list_for_each() iterates over a list from the beginning to the end of the list
	list_for_each(p, child){
		numChild++;
	}

	return numChild;
}

int __init tree_info_init(void)
{
	//char *state[] = {"runnable", "unrunnable", "stopped"};
	printk(KERN_INFO "Entering Tree info module");

	if(mypid == -1)
		task = current;
	else
	{
		pid_struct = find_get_pid(mypid);
		task = pid_task(pid_struct,PIDTYPE_PID);
	}

	for(task=task; task!=&init_task; task=task->parent)	//current is a macro which points to the current task / process
	{
	printk("NAME: %s | PID: %d | STATE: %ld | NICE: %d | CHILDREN: %d |\n",	task->comm , 
										task->pid, 
										task->state, 				// -1: unrunnable, 0: runnable, >0: stopped
										task_nice(task), 			// returns nice value of task
										get_numChild(&task->children));
	}

	return 0;
}

void __exit tree_info_exit(void)
{

	printk(KERN_INFO "Exiting Tree info module");
}

module_init(tree_info_init);
module_exit(tree_info_exit);







