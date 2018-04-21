#include "../main.h"
#include "./tasks/comm_task.h"
#include "./utils/pack_msg.h"

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

extern TaskHandle_t mainTask_handle;
extern QueueHandle_t uv_comm_Queue;
extern QueueHandle_t main_comm_Queue;
extern QueueHandle_t pressure_comm_Queue;

void comm_task(void *pvParameters)
{
    client_pack_t* client_packet = (client_pack_t*)malloc(sizeof(client_pack_t));

    //BaseType_t ret;
    msg_pack_t* packReceived;
    uint32_t comm_ReceivedValue;

    task_status_t tstatus;
    while(1)
    {
        //send heartbeat periodically
        xTaskNotifyWait(0xFFFFFFFF,
                        comm_ReceivedValue,
                        &comm_ReceivedValue,
                        portMAX_DELAY);

        //receive sensor data
        xQueueReceive( uv_comm_Queue, &packReceived, 0U );

        //pack data here
        client_packet->clientInfo = CLIENT_ID;

        if(packReceived->source == SENSOR_UV)
        {
            client_packet->uv_payLoad = packReceived->payload;
        }


        xQueueReceive( pressure_comm_Queue, &packReceived, 0U );

        if(packReceived->source == SENSOR_PRESSURE)
        {
            client_packet->pr_payLoad = packReceived->payload;
        }

        //get task status from main task
        //queue receive here
        xQueueReceive( main_comm_Queue, &tstatus, 0U );
        client_packet->clientStatus = tstatus;

        //how data will be logged in bbg
        // client id:
        // client status: alive tasks [] | dead tasks [] |
        // sensor source | payload | unit | for uv sensor
        // sensor source | payload | unit | for pressure sensor

#ifdef SERIAL_DEBUG
        //print string in terminal
        UARTprintf("\nclient id: %d\n", client_packet->clientInfo);

        char uv_alive, uv_dead, pr_alive, pr_dead;
        if(tstatus & UV_ALIVE){
            uv_alive = 'U';
            uv_dead = ' ';
        }
        else{
            uv_alive = ' ';
            uv_dead = 'U';
        }

        if(tstatus & PRESSURE_ALIVE){
            pr_alive = 'P';
            pr_dead = ' ';
        }
        else{
            pr_alive = ' ';
            pr_dead = 'P';
        }
        UARTprintf("client status: alive [%c %c] | dead [%c %c]\n", uv_alive, pr_alive, uv_dead, pr_dead);


        uint32_t dec = (int)(client_packet->uv_payLoad);
        int frac = (client_packet->uv_payLoad - dec) * 100;
        UARTprintf("sensor: [UV] | [%d.%d] | [?]\n", dec, frac);

        dec = (int)(client_packet->pr_payLoad);
        frac = (client_packet->pr_payLoad - dec) * 100;
        UARTprintf("sensor: [PR] | [%d.%d] | [?]\n", dec, frac);
#endif

        //send client_packet struct to bbg from here

    }
}



