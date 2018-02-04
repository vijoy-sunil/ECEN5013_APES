/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 01/30/2018
* DESCRITPTION	: This is a kernel module that user a kernel timer
		  (500 ms by default) to call a function periodically
		  that writes to the kernel log buffer,
		 
* HEADER FILES	: timer_module.h
****************************************************************/

/*
 * REFERENCE: https://www.ibm.com/developerworks/library/l-timers-list/
*/

#include "timer_module.h"

// 'cnt' keeps count of how many times the timer has fired
static int cnt = 0; 	
static char *name = "Vijoy Sunil Kumar";
static int timer_interval = 500;

// module parameters
module_param(name, charp, S_IRUGO);
module_param(timer_interval, int, S_IRUGO);

// the call back function is called every time the timer fires
void my_timer_callback( unsigned long data)
{
	cnt = cnt + 1;
	printk(KERN_INFO "%s: %d", name, cnt);
	mod_timer(&ktimer, jiffies+ msecs_to_jiffies(timer_interval));	
}

int __init timer_init(void)
{
	int rt;
	setup_timer(&ktimer, my_timer_callback, 0);
	printk(KERN_INFO "Timer INSTALL");
	rt = mod_timer(&ktimer, jiffies+ msecs_to_jiffies(timer_interval));

	if (rt)
		printk(KERN_ALERT "Error setting timer expiration");

	return 0;
}

void __exit timer_exit(void)
{
	del_timer(&ktimer);
	printk(KERN_INFO "Timer UNINSTALL");
}

module_init(timer_init);
module_exit(timer_exit);
