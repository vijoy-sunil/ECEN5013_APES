
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
#define CLIENT_ID               (0x00000001)

//comment to turn off socket comm
//#define USE_SOCKET_CLIENT

//comment to turn off i2c_client
#define USE_UART_CLIENT

// display payload from sensor tasks on terminal
#define SERIAL_DEBUG

//comment to turnoff i2c and run code with dummy data
#define UV_ON
#define PRESSURE_ON

// other Macros
#define SYSTEM_CLOCK            (120000000)
#define TIMER_PERIOD            (pdMS_TO_TICKS(1000))

#define UV_REQUEST_HB           (0x00000001)
#define PRESSURE_REQUEST_HB     (0x00000002)
#define COMM_WAKE_UP            (0x00000004)
#define MAIN_REQUEST_HB         (0x00000008)
#define SOCKET_WAKE_UP          (0x00000010)
#define SOCKET_TASK_READY       (0x00000020)
#define MAIN_WAKE_UP            (0x00000040)

#define MAIN_COMM_QUEUE_LENGTH  (1)
#define TIMEOUT                 (2)
typedef enum{
    ALL_DEAD = 0,
    UV_ALIVE = 1,
    UV_DEAD = 2,
    PRESSURE_ALIVE = 4,
    PRESSURE_DEAD = 8
}task_status_t;

#endif /* MAIN_H_ */
