/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 01/30/2018
* DESCRITPTION	: System call definition that copies data from 
		  user space to kernel space, sorts them from 
		  largest to smallest and copies it back to user 
		  space.
		  
* HEADER FILES	: syscall_sorter.h
****************************************************************/
#include "syscall_sorter.h"

SYSCALL_DEFINE3(sorter, unsigned long*, src, unsigned long*, dest, unsigned long, length)
{
	int32_t *buffer;
	int32_t tmp, i, j;

	printk(KERN_INFO "ENTER: sorter system call");
	printk(KERN_INFO "Buffer size: %ld", length);

	buffer = kmalloc(length *sizeof(int32_t), GFP_KERNEL);
	if(!buffer)
	{
		printk(KERN_ALERT "FAIL: kmalloc() !!!");
		return -EFAULT;
	}
	
	printk(KERN_INFO "Copying buffer from user space to kernel space");
	if(copy_from_user(buffer, src, length *sizeof(int32_t)))
	return -EFAULT;

	printk(KERN_INFO "Sorting buffer");

	for(i = 0; i < length; i++)
	{
		for(j = i+1; j < length; j++)
		{
			if(*(buffer + i) < *(buffer + j))
			{
				tmp = *(buffer + i);
				*(buffer + i) = *(buffer + j);
				*(buffer + j) = tmp;
			}
		}
	}

	printk(KERN_INFO "Sorting complete");

	printk(KERN_INFO "Copying buffer from kernel space to user space");
	if(copy_to_user(dest, buffer, length *sizeof(int32_t)))
		return -EFAULT;

	printk(KERN_INFO "Free allocated memory");
	kfree(buffer);
	printk(KERN_INFO "EXIT: sorter system call");

	return 0;

}
