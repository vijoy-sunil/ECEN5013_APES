
#include "../main.h"

volatile int status_check_time = 0;

extern TimerHandle_t MyTimer;

extern TaskHandle_t uvTask_handle;
extern TaskHandle_t pressureTask_handle;
extern TaskHandle_t commTask_handle;
extern TaskHandle_t mainTask_handle;
 /*----------------------------------------------------------*/
 /* Define a callback function that will be used by multiple timer
 instances.  The callback function does nothing but count the number
 of times the associated timer expires, and stop the timer once the
 timer has expired 10 times.  The count is saved as the ID of the
 timer. */

 void vTimerCallback( TimerHandle_t xTimer )
 {
    const uint32_t ulMaxExpiryCountBeforeStopping = 1;
    uint32_t ulCount;
    uint8_t signal;
    //BaseType_t ret;

    /* Optionally do something if the pxTimer parameter is NULL. */
    //configASSERT( pxTimer );

    /* The number of times this timer has expired is saved as the
    timer's ID.  Obtain the count. */
    ulCount = ( uint32_t ) pvTimerGetTimerID( MyTimer );

    /* Increment the count, then test to see if the timer has expired
    ulMaxExpiryCountBeforeStopping yet. */
    ulCount++;

    if( ulCount == ulMaxExpiryCountBeforeStopping)
    {
        // ask all 3 tasks to send heartbeat to main task
        signal =UV_REQUEST_HB;
        xTaskNotify(uvTask_handle, signal, eSetBits);


        signal =PRESSURE_REQUEST_HB;
        xTaskNotify(pressureTask_handle, signal, eSetBits);


        signal =COMM_REQUEST_HB;
        xTaskNotify(commTask_handle, signal, eSetBits);

        signal =MAIN_REQUEST_HB;
        xTaskNotify(mainTask_handle, signal, eSetBits);

        //UARTprintf("hb request timer fired\r\n");

        //keep track of 3 timer fires - to check hb miss

        status_check_time++;

        ulCount = 0;
    }
       /* Store the incremented count back into the timer's ID field
       so it can be read back again the next time this software timer
       expires. */
       vTimerSetTimerID( MyTimer, ( void * ) ulCount );

 }
