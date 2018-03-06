/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 03/02/2018
* DESCRITPTION	: This program creates a kernel module that allows two threads
		  to communicate via queues (kfifo). The first thread sends
		  information to the second thread on a timed interval through the fifo.
		  The second thread should take data from the kfifo and print it to the 
		  kernel logger. The info comprises of PID and vruntime of the previous, 
		  current, and next PID 
		 
* HEADER FILES	: mykthreads.h
****************************************************************/


#include "mykthreads.h"


static int child_one(void *unused)
{
	// kthread_should_stop() function returns non-zero value, if there is any outstanding ‘stop’ request. 
	// Thread should invoke this call periodically and if it returns true, it should do the required clean up and exit
	
	while (!kthread_should_stop())
	{
		printk(KERN_ALERT "child_one thread Running\n");
		//lock mutex
		mutex_lock(&lock_my_fifo);
		kfifo_put(&my_fifo, current);
	


		//unlock mutex
		mutex_unlock(&lock_my_fifo);
		ssleep(2);
	}
	printk(KERN_ALERT "child_one thread Stopping\n");
	do_exit(0);
	return 0;
}

static int child_two(void *unused)
{
	while (!kthread_should_stop())
	{
		while (!kfifo_is_empty(&my_fifo)) 
		{
			printk(KERN_ALERT "child_two thread Running\n");
			//lock mutex
			mutex_lock(&lock_my_fifo);
			kfifo_get(&my_fifo, &fifo_data);
			printk(KERN_INFO "Previous PID: %d | vruntime: %llu\n", list_prev_entry(fifo_data, tasks)->pid, 
										list_prev_entry(fifo_data, tasks)->se.vruntime);
	
			printk(KERN_INFO "Current PID: %d | vruntime: %llu\n",	fifo_data->pid, 
										fifo_data->se.vruntime);
	
			printk(KERN_INFO "Next PID: %d | vruntime: %llu\n",  	list_next_entry(fifo_data, tasks)->pid, 
										list_next_entry(fifo_data, tasks)->se.vruntime	);
			//unlock mutex
			mutex_unlock(&lock_my_fifo);
			//ssleep(10);
		}
	}
	printk(KERN_ALERT "child_two thread Stopping\n");
	do_exit(0);
	return 0;
}


int __init kthreads_init(void)
{

	printk(KERN_INFO "Initializing kthreads module");

	INIT_KFIFO(my_fifo);

	// when we create the thread with kthread_create(), it creates the thread in sleep state 
	// and thus nothing is executed. We have a API wake_up_process() 

	thread1 = kthread_create(child_one, NULL, "child_thread_one");
	if (thread1 != NULL)
		wake_up_process(thread1);
	
	else
		printk(KERN_ALERT "ERROR: child_thread_one creation\n");

	thread2 = kthread_create(child_two, NULL, "child_thread_two");
	if (thread2 != NULL)
		wake_up_process(thread2);
	else
		printk(KERN_ALERT "ERROR: child_thread_two creation\n");


	return 0;
}

void __exit kthreads_exit(void)
{

	printk(KERN_INFO "Exiting ktheads module");
	if (thread1)
		kthread_stop(thread1);

	if (thread2)
		kthread_stop(thread2);

}

module_init(kthreads_init);
module_exit(kthreads_exit);





