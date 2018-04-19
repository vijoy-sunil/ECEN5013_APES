#include "main.h"
#include "./tasks/comm_task.h"
#include "./tasks/uv_task.h"
#include "./tasks/pressure_task.h"
#include "./comms/i2c_drive.h"

void main_task(void *pvParameters);
TaskHandle_t mainTask_handle;
TaskHandle_t uvTask_handle;
TaskHandle_t pressureTask_handle;
TaskHandle_t commTask_handle;

TimerHandle_t MyTimer;
void vTimerCallback( TimerHandle_t xTimer );

QueueHandle_t main_comm_Queue = NULL;

uint32_t output_clock_rate_hz;
task_status_t get_task_status(void);

extern volatile int status_check_time;

int main(void)
{
    // set clock frequency
    output_clock_rate_hz = MAP_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                                SYSTEM_CLOCK);

    // Configure the UART.
    ConfigureUART();

    // Configure the I2C
    ConfigureI2C();

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

   // create tasks

    xTaskCreate(main_task, "MAIN-TASK",
                configMINIMAL_STACK_SIZE, NULL, 1, &mainTask_handle);

    xTaskCreate(uv_task, "UV-TASK",
                configMINIMAL_STACK_SIZE, NULL, 1, &uvTask_handle);

    xTaskCreate(pressure_task, "PRESSURE-TASK",
                configMINIMAL_STACK_SIZE, NULL, 1, &pressureTask_handle);

    xTaskCreate(comm_task, "COMM-TASK",
                configMINIMAL_STACK_SIZE, NULL, 1, &commTask_handle);

    vTaskStartScheduler();
    return 0;
}

void main_task(void *pvParameters)
{
    task_status_t tstatus;
    uint32_t main_ReceivedValue;
    main_comm_Queue = xQueueCreate( MAIN_COMM_QUEUE_LENGTH, sizeof(task_status_t) );
    UARTprintf("Main task running\n");
    while(1)
    {
        xTaskNotifyWait(0xFFFFFFFF,
                        main_ReceivedValue,
                        &main_ReceivedValue,
                        portMAX_DELAY);

        //functions
        //get heartbeat periodically
        tstatus = get_task_status();

#ifdef SERIAL_DEBUG
        if(tstatus & UV_ALIVE)
            UARTprintf("<UV>\r\n");
        if(tstatus & PRESSURE_ALIVE)
            UARTprintf("<PR>\r\n");
        if(tstatus & COMM_ALIVE)
            UARTprintf("<COMM>\r\n");
#endif

        //send status of other tasks to comm task - alive/dead
        //send data to comm task
        xQueueSend( main_comm_Queue, &tstatus, portMAX_DELAY);

    }
}

//return dead or alive based upon heartbeat (3 missed heatbeat)
task_status_t get_task_status(void)
{
    uint32_t ulReceivedValue = 0;
    task_status_t state;
    static int uv_hb_cnt, comm_hb_cnt, pressure_hb_cnt;

    //receive heartbeat
    xTaskNotifyWait(0xFFFFFFFF,
                    ulReceivedValue,
                    &ulReceivedValue,
                    portMAX_DELAY);

    if(ulReceivedValue & UV_SEND_HB)
        uv_hb_cnt++;


    if(ulReceivedValue & PRESSURE_SEND_HB)
        pressure_hb_cnt++;


    if(ulReceivedValue & COMM_SEND_HB)
        comm_hb_cnt++;

    //clear notification
    ulReceivedValue = 0;


    //UARTprintf("uv: %d, pressure: %d, comm: %d, fired: %d\r\n", uv_hb_cnt, pressure_hb_cnt, comm_hb_cnt,status_check_time );
    //check for 3 missed heartbeats
    state = ALL_DEAD;

    if(status_check_time == NUM_MISS_HB){
        status_check_time = 0;

        if(uv_hb_cnt == 0)
            state |= UV_DEAD;
        else
            state |= UV_ALIVE;

        if(pressure_hb_cnt == 0)
            state |= PRESSURE_DEAD;
        else
            state |= PRESSURE_ALIVE;

        if(comm_hb_cnt == 0)
            state |= COMM_DEAD;
        else
            state |= COMM_ALIVE;

        uv_hb_cnt = 0;
        pressure_hb_cnt = 0;
        comm_hb_cnt = 0;

    }
    return state;
}

