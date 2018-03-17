
#ifndef SIGNALS_H
#define SIGNALS_H

#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int ConfigTimerTemp();
int ConfigLightSetup();
void Signal_interrupt(int sig);
void temperature_signal(int sig);
void light_signal(int sig);

#define TEMPERATURE_SIGNAL     (SIGRTMAX)
#define LIGHT_SIGNAL    (SIGRTMAX-1)

#define LIGHT_HB_SIG (SIGRTMAX-2)
#define TEMPERATURE_HB_SIG  (SIGRTMAX-3)
#define LOGGER_HB_SIG  (SIGRTMAX-4)

#define NSEC_FREQUENCY (1000000000)

#define TEMPERATURE_SIGNAL_IPC (SIGRTMAX-5)
#define LIGHT_SIGNAL_IPC (SIGRTMAX-6)

#define LOGGER_SIGNAL (SIGRTMAX-7)



sig_atomic_t light_close;
sig_atomic_t logger_close;
sig_atomic_t temp_close;
sig_atomic_t app_close;



#endif
