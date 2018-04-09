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

void blink1_LED(void *pvParameters);
void blink2_LED(void *pvParameters);
void vTimerCallback( TimerHandle_t xTimer );

TimerHandle_t MyTimer;
QueueHandle_t LED1Queue = NULL;
QueueHandle_t LED2Queue = NULL;

uint32_t output_clock_rate_hz;
unsigned long ulReceivedValue;
uint32_t toggle_led1 = 0;
uint32_t toggle_led2 = 0;

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
    // configure uart for log
    // Initialize the UART.
    ConfigureUART();

    // configure leds
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    // Enable the GPIO pins for the LED D2 (PN0).
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    // Enable the GPIO pins for the LED D1 (PN1).
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


   /* Create the queue. */
   UARTprintf("Creating Message queue\r\n");
   LED1Queue = xQueueCreate( SIGNAL_QUEUE_LENGTH, sizeof( unsigned long ) );
   LED2Queue = xQueueCreate( SIGNAL_QUEUE_LENGTH, sizeof( unsigned long ) );

   // create tasks
    xTaskCreate(blink1_LED, "blink1",
                configMINIMAL_STACK_SIZE, NULL, 2, NULL);

    xTaskCreate(blink2_LED, "blink2",
                configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    vTaskStartScheduler();
    return 0;
}


// 2Hz task, 500 ms
void blink1_LED(void *pvParameters)
{
    toggle_led1 = GPIO_PIN_1;
    while(1)
    {
        /* Wait until something arrives in the queue - this task will block
        indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
        FreeRTOSConfig.h. */
        //UARTprintf("blink1_LED task Waiting for timer signal\r\n");
        xQueueReceive( LED1Queue, &ulReceivedValue, portMAX_DELAY );

        /*  To get here something must have been received from the queue, but
        is it the expected value?  If it is, toggle the LED. */

        if(ulReceivedValue == LED1_RATE){
            //toggle led1
            UARTprintf( "blink1_LED timer signal received\r\n");
            toggle_led1 ^= 0x00000002;
            ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, toggle_led1);

        }
    }
}

// 4Hz task, 250 ms
void blink2_LED(void *pvParameters)
{
    toggle_led2 = GPIO_PIN_0;
    while(1)
    {
        /* Wait until something arrives in the queue - this task will block
        indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
        FreeRTOSConfig.h. */
        //UARTprintf("blink2_LED task Waiting for timer signal\r\n");
        xQueueReceive( LED2Queue, &ulReceivedValue, portMAX_DELAY ); //send timer isr ulcount

        /*  To get here something must have been received from the queue, but
        is it the expected value?  If it is, toggle the LED. */


        if(ulReceivedValue == LED2_RATE){
            //toggle led2
            UARTprintf( "blink2_LED timer signal received\r\n");
            toggle_led2 ^= 0x00000001;
            ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, toggle_led2);

        }
    }
}


