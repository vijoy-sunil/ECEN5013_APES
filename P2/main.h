
#ifndef MAIN_H_
#define MAIN_H_

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

// TivaWare includes
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

//unique identifier
#define CLIENT_ID               (0x00)

// display payload from sensor tasks on terminal
//#define SERIAL_DEBUG

// other Macros
#define SYSTEM_CLOCK            (120000000)
#define TIMER_PERIOD            (pdMS_TO_TICKS(1000))    //1000 ms
#define HB_QUEUE_LENGTH         (1)

#define UV_REQUEST_HB           (0x01)
#define PRESSURE_REQUEST_HB     (0x02)
#define COMM_REQUEST_HB         (0x04)
#define MAIN_REQUEST_HB         (0x08)

#define NUM_MISS_HB             (4)                      //heartbeat check every 3 seconds
#define MAIN_COMM_QUEUE_LENGTH  (1)

typedef enum{
    ALL_DEAD = 0,
    UV_ALIVE = 1,
    UV_DEAD = 2,
    PRESSURE_ALIVE = 4,
    PRESSURE_DEAD = 8,
    COMM_ALIVE = 16,
    COMM_DEAD = 32
}task_status_t;

#endif /* MAIN_H_ */
