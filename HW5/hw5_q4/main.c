// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include <stdint.h>
#include <stdbool.h>
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

#include "main.h"


TimerHandle_t MyTimer;
QueueHandle_t T1Queue = NULL;
QueueHandle_t T2Queue = NULL;

QueueHandle_t tickQueue = NULL;

uint32_t output_clock_rate_hz;
uint32_t toggle_led1 = 0;

TaskHandle_t task1_handle, task2_handle, task3_handle;
void task1(void *pvParameters);
void task2(void *pvParameters);
void task3(void *pvParameters);
void vTimerCallback( TimerHandle_t xTimer );

// Configure the UART and its pins
void ConfigureUART(void)
{
    // Enable the GPIO Peripheral used by the UART.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Enable UART0
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Configure GPIO Pins for UART mode.
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, output_clock_rate_hz);
}

int main(void)
{
    // set clock frequency
    output_clock_rate_hz = MAP_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                                SYSTEM_CLOCK);

    // Configure the UART.
    ConfigureUART();

    // configure led
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    // configure timer
    /* Create then start timer.  Starting the timer before
    the RTOS scheduler has been started means the timers will start
    running immediately that the RTOS scheduler starts. */

    UARTprintf("Creating timer \r\n");
    MyTimer = xTimerCreate
                   ( /* Just a text name, not used by the RTOS
                    kernel. */
                    "Timer",
                    /* The timer period in ticks, must be
                    greater than 0. */
                    TIMER_PERIOD,
                    /* The timers will auto-reload themselves
                    when they expire. */
                    pdTRUE,
                    /* The ID is used to store a count of the
                    number of times the timer has expired, which
                    is initialised to 0. */
                    ( void * ) 0,
                    /* Each timer calls the same callback when
                    it expires. */
                    vTimerCallback
                   );

    if( MyTimer == NULL )
    {
        /* The timer was not created. */
        UARTprintf("Creating timer failed \r\n");
    }
    else
    {
        /* Start the timer.  No block time is specified, and
        even if one was it would be ignored because the RTOS
        scheduler has not yet been started. */
        if( xTimerStart( MyTimer, 0 ) != pdPASS )
        {
            UARTprintf("The timer could not be set into the Active state\r\n");
        }
    }

    /* Create the queues. */
    UARTprintf("Creating Message queue\r\n");

    //timer to task queues
    T1Queue = xQueueCreate( STRING_QUEUE_LENGTH, sizeof(unsigned char) );
    T2Queue = xQueueCreate( SIGNAL_QUEUE_LENGTH, sizeof(unsigned char) );

    //notify task3
    tickQueue = xQueueCreate( SIGNAL_QUEUE_LENGTH, sizeof(unsigned long) );

   // create tasks
    xTaskCreate(task1, "TASK-1",
                configMINIMAL_STACK_SIZE, NULL, 2, &task1_handle);

    xTaskCreate(task2, "TASK-2",
                configMINIMAL_STACK_SIZE, NULL, 3, &task2_handle);

    xTaskCreate(task3, "TASK-3",
                configMINIMAL_STACK_SIZE, NULL, 1, &task3_handle);

    vTaskStartScheduler();
    return 0;
}


// 2Hz task
void task1(void *pvParameters)
{
    unsigned char ulReceivedValue;
    while(1)
    {
        /* Wait until something arrives in the queue - this task will block
        indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
        FreeRTOSConfig.h. */
        xQueueReceive( T1Queue, &ulReceivedValue, portMAX_DELAY );

        /*  To get here something must have been received from the queue, but
        is it the expected value?  If it is, toggle the LED. */

        if(ulReceivedValue == TASK1_RATE){
            //signal task3

            UARTprintf( "task1 notifying task3 - TOGGLE_LED\r\n");

            //notify task3
            xTaskNotify(task3_handle, TOGGLE_LED, eSetBits);
        }

    }
}

// 4Hz task
const char *str = "Event Driven UI";
void task2(void *pvParameters)
{
    TickType_t tick_cnt;
    unsigned char ulReceivedValue;
    while(1)
    {
        /* Wait until something arrives in the queue - this task will block
        indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
        FreeRTOSConfig.h. */
        xQueueReceive( T2Queue, &ulReceivedValue, portMAX_DELAY );

        /*  To get here something must have been received from the queue, but
        is it the expected value?  If it is, toggle the LED. */

        if(ulReceivedValue == TASK2_RATE){
            //signal task3

            UARTprintf( "task2 notifying task3 - LOG_STRING\r\n");
            //pass tick count
            tick_cnt = xTaskGetTickCount();
            xQueueSend( tickQueue, &tick_cnt, 0U );
            //notify task3
            xTaskNotify(task3_handle, LOG_STRING, eSetBits);
        }

    }
}


void task3(void *pvParameters)
{
    uint32_t notifyReceivedValue;
    toggle_led1 = GPIO_PIN_1;
    uint32_t tickValue;
    while(1)
    {
        xTaskNotifyWait(0xFFFFFFFF,
                          notifyReceivedValue,
                          &notifyReceivedValue,
                          portMAX_DELAY);

        if(notifyReceivedValue & TOGGLE_LED){
            //toggle led1
            ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, toggle_led1);
            toggle_led1 ^= 0x00000002;
        }

        if(notifyReceivedValue & LOG_STRING){
            //receive string from queue
            xQueueReceive( tickQueue, &tickValue, portMAX_DELAY );
            //print string
            UARTprintf("tick count: %d \r\n", tickValue);
        }
    }
}
