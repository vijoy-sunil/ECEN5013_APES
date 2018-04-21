
#include "../main.h"

extern TimerHandle_t MyTimer;

extern TaskHandle_t uvTask_handle;
extern TaskHandle_t pressureTask_handle;
 /*----------------------------------------------------------*/
 /* Define a callback function that will be used by multiple timer
 instances.  The callback function does nothing but count the number
 of times the associated timer expires, and stop the timer once the
 timer has expired 10 times.  The count is saved as the ID of the
 timer. */

 void vTimerCallback( TimerHandle_t xTimer )
 {
    //const uint32_t ulMaxExpiryCountBeforeStopping = 2;
    uint32_t ulCount;
    uint32_t signal;
    //BaseType_t ret;

    /* Optionally do something if the pxTimer parameter is NULL. */
    //configASSERT( pxTimer );

    /* The number of times this timer has expired is saved as the
    timer's ID.  Obtain the count. */
    ulCount = ( uint32_t ) pvTimerGetTimerID( MyTimer );

    /* Increment the count, then test to see if the timer has expired
    ulMaxExpiryCountBeforeStopping yet. */
    ulCount++;

    if( ulCount % 2 ==  0)
    {
        signal =PRESSURE_REQUEST_HB;
        xTaskNotify(pressureTask_handle, signal, eSetBits);

        ulCount = 0;
    }

    else if( ulCount % 2 ==  1)
    {
        signal =UV_REQUEST_HB;
        xTaskNotify(uvTask_handle, signal, eSetBits);
    }

    /* Store the incremented count back into the timer's ID field
    so it can be read back again the next time this software timer
    expires. */
    vTimerSetTimerID( MyTimer, ( void * ) ulCount );

 }
