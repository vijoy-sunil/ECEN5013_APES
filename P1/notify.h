#ifndef SIGNALS_H
#define SIGNALS_H
#include <signal.h>

#define SIGTEMP     (SIGRTMAX)
#define SIGLIGHT    (SIGRTMAX-1)

#define LIGHT_SIG_HEARTBEAT (SIGRTMAX-2)
#define TEMPERATURE_SIG_HEARTBEAT  (SIGRTMAX-3)
#define SIGLOG_HB  (SIGRTMAX-4)

#define SIGTEMP_IPC (SIGRTMAX-5)
#define SIGLIGHT_IPC (SIGRTMAX-6)

#define SIGLOG (SIGRTMAX-7)

#define SIGSOCKET_HB  (SIGRTMAX-8)


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
*@brief:sets timer and signal handler for timer notification for temp task
*@param:void
*@return: int success/failure
*/
int setTempTimer();

/**
*@brief:sets timer and signal handler for light task
*@param:void
*@return: int success/failure
*/
int setLightTimer();


/**
*@brief:Signal handler for temp task
*sets the global flag atomically and signals the temp task throught a condition *variable to read next data
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
