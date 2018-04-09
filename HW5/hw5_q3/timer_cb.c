
#include "FreeRTOS.h"
#include "timers.h"
#include "queue.h"

#include <stdbool.h>
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "main.h"

extern unsigned long ulReceivedValue;
extern TimerHandle_t MyTimer;
extern QueueHandle_t LED1Queue;
extern QueueHandle_t LED2Queue;

 /*----------------------------------------------------------*/
 /* Define a callback function that will be used by multiple timer
 instances.  The callback function does nothing but count the number
 of times the associated timer expires, and stop the timer once the
 timer has expired 10 times.  The count is saved as the ID of the
 timer. */

 void vTimerCallback( TimerHandle_t xTimer )
 {
    const uint32_t ulMaxExpiryCountBeforeStopping = 2;
    uint32_t ulCount, signal;

    /* Optionally do something if the pxTimer parameter is NULL. */
    //configASSERT( pxTimer );

    /* The number of times this timer has expired is saved as the
    timer's ID.  Obtain the count. */
    ulCount = ( uint32_t ) pvTimerGetTimerID( MyTimer );

    /* Increment the count, then test to see if the timer has expired
    ulMaxExpiryCountBeforeStopping yet. */
    ulCount++;

    /* If the timer has expired */
    if( ulCount == 1 )  //4Hz
    {
        /* Send to the queue - causing the queue receive task to unblock and
        toggle the LED.  0 is used as the block time so the sending operation
        will not block - it shouldn't need to block as the queue should always
        be empty at this point in the code. */
        UARTprintf("250ms timer fire\r\n");
        signal =250;
        xQueueSend( LED2Queue, &signal, 0U );
    }
    else if( ulCount == ulMaxExpiryCountBeforeStopping) //2Hz
    {
        UARTprintf("500ms timer fire\r\n");
        signal =500;
        xQueueSend( LED1Queue, &signal, 0U );
        signal =250;
        xQueueSend( LED2Queue, &signal, 0U );
        ulCount = 0;
    }
       /* Store the incremented count back into the timer's ID field
       so it can be read back again the next time this software timer
       expires. */
       vTimerSetTimerID( MyTimer, ( void * ) ulCount );

 }
