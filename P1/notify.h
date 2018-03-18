/***************************************************************
* AUTHOR  : Praveen Gnanaseakran
* DATE    : 03/08/2018
* DESCRITPTION  : NOTIFY FILES
                  
* SOURCE FILES  : notify.c
****************************************************************/

#include <signal.h>

#define FREQ_NSEC (1000000000)

#define TEMPERATURE_SIGNAL_OPT     (SIGRTMAX)
#define LIGHT_SIGNAL_OPT    (SIGRTMAX-1)

#define LIGHT_SIG_HEARTBEAT (SIGRTMAX-2)
#define TEMPERATURE_SIG_HEARTBEAT  (SIGRTMAX-3)
#define LOGGER_SIG_HEARTBEAT  (SIGRTMAX-4)

#define TEMPSIGNAL_PACKET (SIGRTMAX-5)
#define LIGHTSIGNAL_PACKET (SIGRTMAX-6)

#define LOGGER_SIG (SIGRTMAX-7)

#define SOCKET_SIG_HEARTBEAT  (SIGRTMAX-8)


void SIGNAL_INTERRUPT_HANDL(int sig);



void temp_sig_handler(int sig);


void light_sig_handler(int sig);


sig_atomic_t light_close_flag;
sig_atomic_t logger_close_flag;
sig_atomic_t temperature_close_flag;
sig_atomic_t socket_close_flag;
sig_atomic_t application_close_flag;

void SIGNAL_INTERRUPT_HANDL(int sig);

int Temp_init_timer();

int timer_init_light();

void temp_sig_handler(int sig);

void light_sig_handler(int sig);
