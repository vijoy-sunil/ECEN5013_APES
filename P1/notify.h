#ifndef SIGNALS_H
#define SIGNALS_H
#include <signal.h>

#define TEMPERATURE_SIGNAL_OPT     (SIGRTMAX)
#define LIGHT_SIGNAL_OPT    (SIGRTMAX-1)

#define LIGHT_SIG_HEARTBEAT (SIGRTMAX-2)
#define TEMPERATURE_SIG_HEARTBEAT  (SIGRTMAX-3)
#define LOGGER_SIG_HEARTBEAT  (SIGRTMAX-4)

#define TEMPSIGNAL_PACKET (SIGRTMAX-5)
#define LIGHTSIGNAL_PACKET (SIGRTMAX-6)

#define LOGGER_SIG (SIGRTMAX-7)

#define SOCKET_SIG_HEARTBEAT  (SIGRTMAX-8)


sig_atomic_t light_close_flag;
sig_atomic_t logger_close_flag;
sig_atomic_t temperature_close_flag;
sig_atomic_t socket_close_flag;
sig_atomic_t application_close_flag;

/**
*@brief:Signal handler for SIGINT
*clears the global flag atomically to allow tasks to exit from while(1) loop and close *ques, file descriptors etc before exiting
*@param:signal no.
*@return: no returns
*/
void SIGNAL_INTERRUPT_HANDL(int sig);


/**
*@brief:sets timer and signal handler for timer notification for temperature task
*@param:void
*@return: int success/failure
*/
int Temp_init_timer();

/**
*@brief:sets timer and signal handler for light task
*@param:void
*@return: int success/failure
*/
int timer_init_light();


/**
*@brief:Signal handler for temperature task
*sets the global flag atomically and signals the temperature task throught a condition *variable to read next data
*@param:signal no.
*@return: no returns
*/
void temp_sig_handler(int sig);

/**
*@brief:Signal handler for light task
*sets the global flag atomically and signals the light task throught a *condition variable to read next data
*@param:signal no.
*@return: no returns
*/
void light_sig_handler(int sig);


#endif
