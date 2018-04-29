#include "main.h"
#include "./comm/uart_client.h"
#include "./tasks/comm_task.h"
#include "./tasks/uv_task.h"
#include "./tasks/pressure_task.h"
#include "./comm/i2c_drive.h"

#include "FreeRTOS_IP.h"

void main_task(void *pvParameters);
TaskHandle_t mainTask_handle;
TaskHandle_t uvTask_handle;
TaskHandle_t pressureTask_handle;
TaskHandle_t commTask_handle;

TimerHandle_t MyTimer;
void vTimerCallback( TimerHandle_t xTimer );

QueueHandle_t main_comm_Queue = NULL;

uint32_t output_clock_rate_hz;
int main_timeout = TIMEOUT;
task_status_t get_task_status(uint32_t main_ReceivedValue);

extern volatile int status_check_time;

int main(void)
{
    // set clock frequency
    output_clock_rate_hz = MAP_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                                SYSTEM_CLOCK);
    // Configure UART clent
    ConfigureUART_client();

    // Configure the UART.
    ConfigureUART();

    // Configure the I2C
    ConfigureI2C();

    // configure leds - heartbeats for sensors
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    // configure timer
    /* Create then start timer.  Starting the timer before
    the RTOS scheduler has been started means the timers will start
    running immediately that the RTOS scheduler starts. */

    UARTprintf("Creating timer \r\n");
    MyTimer = xTimerCreate("Timer", TIMER_PERIOD, pdTRUE,( void * ) 0,vTimerCallback);

    if( MyTimer == NULL )
    {
        // The timer was not created.
        UARTprintf("Creating timer failed \r\n");
    }
    else
    {
        // Start the timer.  No block time is specified, and
        // even if one was it would be ignored because the RTOS
        // scheduler has not yet been started.
        if( xTimerStart( MyTimer, 0 ) != pdPASS )
        {
            UARTprintf("The timer could not be set into the Active state\r\n");
        }
    }

#ifdef USE_SOCKET_CLIENT
    /* The MAC address array is not declared const as the MAC address will
    normally be read from an EEPROM and not hard coded (in real deployed
    applications).*/
    static uint8_t ucMACAddress[ 6 ] = { 0x00, 0x1A, 0xB6, 0x03, 0x35, 0xB0 };

    /* Define the network addressing.  These parameters will be used if either
    ipconfigUDE_DHCP is 0 or if ipconfigUSE_DHCP is 1 but DHCP auto configuration
    failed. */
    static const uint8_t ucIPAddress[ 4 ] = { 10, 10, 10, 200 };
    static const uint8_t ucNetMask[ 4 ] = { 255, 255, 255, 0 };
    static const uint8_t ucGatewayAddress[ 4 ] = { 192, 168, 0, 1 };

    /* The following is the address of an OpenDNS server. */
    static const uint8_t ucDNSServerAddress[ 4 ] = { 192, 168, 0, 1 };

    /* Initialise the RTOS's TCP/IP stack.  The tasks that use the network
    are created in the vApplicationIPNetworkEventHook() hook function
    below.  The hook function is called when the network connects. */

    BaseType_t ret = FreeRTOS_IPInit( ucIPAddress,
                     ucNetMask,
                     ucGatewayAddress,
                     ucDNSServerAddress,
                     ucMACAddress );

    if(ret == pdPASS)
        UARTprintf("Initializing TCP/IP stack SUCCESS\n");
    else
        UARTprintf("Initializing TCP/IP stack FAILED\n");
#endif

    // create tasks

    xTaskCreate(main_task, "MAIN-TASK",
                configMINIMAL_STACK_SIZE, NULL, 3, &mainTask_handle);

    xTaskCreate(uv_task, "UV-TASK",
                configMINIMAL_STACK_SIZE, NULL, 2, &uvTask_handle);

    xTaskCreate(pressure_task, "PRESSURE-TASK",
                configMINIMAL_STACK_SIZE, NULL, 2, &pressureTask_handle);

    xTaskCreate(comm_task, "COMM-TASK",
                configMINIMAL_STACK_SIZE, NULL, 3, &commTask_handle);
    vTaskStartScheduler();
    return 0;
}

void main_task(void *pvParameters)
{
    task_status_t tstatus;
    uint32_t main_ReceivedValue = 0, signal;
    main_comm_Queue = xQueueCreate( MAIN_COMM_QUEUE_LENGTH, sizeof(task_status_t) );
    UARTprintf("Main task running\n");

    while(1)
    {
        xTaskNotifyWait(0xFFFFFFFF,
                        main_ReceivedValue,
                        &main_ReceivedValue,
                        portMAX_DELAY);

        //reset timeout
        main_timeout = TIMEOUT;

        if(main_ReceivedValue == UV_SEND_HB || main_ReceivedValue == PRESSURE_SEND_HB) //signal from either sensor tasks
        {
            //get heartbeat periodically
            tstatus = get_task_status(main_ReceivedValue);
            //send alive/dead status tasks to comm task
            xQueueSend( main_comm_Queue, &tstatus, 0U);

            signal =COMM_WAKE_UP;
            xTaskNotify(commTask_handle, signal, eSetBits);
            //clear notification
            main_ReceivedValue = 0;
        }

        else //both sensors are dead
        {
            main_ReceivedValue = 0;
            tstatus = ALL_DEAD;
            xQueueSend( main_comm_Queue, &tstatus, 0U);
            signal =COMM_WAKE_UP;
            xTaskNotify(commTask_handle, signal, eSetBits);
        }
    }
}

//return dead or alive based upon heartbeat
task_status_t get_task_status(uint32_t ulReceivedValue)
{
    static task_status_t state = ALL_DEAD;
    static task_status_t prev_state_uv, prev_state_pr;

    static uint32_t toggle_led1 = GPIO_PIN_1;
    static uint32_t toggle_led0 = GPIO_PIN_0;

    if(ulReceivedValue & UV_SEND_HB)
    {
        toggle_led0 ^= 0x00000001;
        ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, toggle_led0);     //led heartbeat

        state |= UV_ALIVE;
        prev_state_uv = UV_ALIVE;
    }
    else if(prev_state_uv == UV_DEAD)
        state |= UV_DEAD;
    else
        prev_state_uv = UV_ALIVE;



    if(ulReceivedValue & PRESSURE_SEND_HB)
    {
        toggle_led1 ^= 0x00000002;
        ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, toggle_led1);     //led heartbeat

        state |= PRESSURE_ALIVE;
        prev_state_pr = PRESSURE_ALIVE;
    }
    else if(prev_state_pr == PRESSURE_DEAD)
        state |= PRESSURE_DEAD;
    else
        prev_state_pr |= PRESSURE_DEAD;

    return state;
}

