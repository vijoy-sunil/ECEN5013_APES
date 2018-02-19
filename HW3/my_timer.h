/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 02/18/2018
* DESCRITPTION	: This file containes the functions
		  init_timer - this initializes the timer with 
		  period_ns from the passed arguments. if the preiod_ns
		  is zero, then it deletes the timer.

		  my_handler - this function is the timer handler function
		  that gets called in every 100 ms (period_ns). 
		  my_handler opens the shared logfile and writes the CPU
		  utillization metric obtained from /proc/stat
		  
****************************************************************/
void init_timer(unsigned long period_ns, void* tinfo);
int my_handler(union sigval arg);


