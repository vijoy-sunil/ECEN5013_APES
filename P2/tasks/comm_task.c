
#include "../main.h"
#include "./tasks/comm_task.h"
#include "./utils/pack_msg.h"
#include "../comm/socket_client.h"

extern uint32_t output_clock_rate_hz;
extern TaskHandle_t socket_clientTask_handle;
int socket_client_task_ready = 0;

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

extern client_packet_t* comm_client_packet;
QueueHandle_t comm_socket_Queue = NULL;
void comm_task(void *pvParameters)
{
    //data transfer queue
    comm_socket_Queue = xQueueCreate( COMM_SOCKET_QUEUE_LENGTH, sizeof(client_pack_t) );
    client_pack_t* client_packet = (client_pack_t*)malloc(sizeof(client_pack_t));
    comm_client_packet = (client_packet_t*)client_packet;

    //BaseType_t ret;
    msg_pack_t* packReceived = (msg_pack_t*)malloc(sizeof(msg_pack_t));
    uint32_t comm_ReceivedValue = 0;

    task_status_t tstatus;
    while(1)
    {
        //send heartbeat periodically
        xTaskNotifyWait(0xFFFFFFFF,
                        comm_ReceivedValue,
                        &comm_ReceivedValue,
                        portMAX_DELAY);

        //notify from uv sensor{
        //receive sensor data
        xQueueReceive( uv_comm_Queue, &packReceived, 0U );

        //pack data here
        client_packet->clientInfo = CLIENT_ID;

        if(packReceived->source == SENSOR_UV)
        {
            client_packet->uv_payLoad = packReceived->payload;
        }
        //}

        //notify from pressure sensor{
        xQueueReceive( pressure_comm_Queue, &packReceived, 0U );

        if(packReceived->source == SENSOR_PRESSURE)
        {
            client_packet->pr_payLoad = packReceived->payload;
        }
        //}

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
        UARTprintf("sensor: [UV] | [%d] | [UVI]\n", dec);

        dec = (int)(client_packet->pr_payLoad);
        uint32_t frac = (client_packet->pr_payLoad - dec) * 100;
        UARTprintf("sensor: [PR] | [%d.%d] | [kPa]\n", dec, frac);
#endif

        //send client_packet struct to bbg from here

#ifdef USE_SOCKET_CLIENT
        uint32_t signal;

        //get notification from network interface
        if(socket_client_task_ready == 1)
        {
            //send client_packet struct thru queue to socket task
            xQueueSend( comm_socket_Queue, &client_packet, 0U);
            //notify socket task
            signal = SOCKET_WAKE_UP;
            xTaskNotify(socket_clientTask_handle, signal, eSetBits);
        }
#endif


#ifdef USE_UART_CLIENT
        int numberOfBytes = sizeof(client_pack_t);
        uart_send_data(UART6_BASE, (const uint8_t*)client_packet,numberOfBytes );
#endif

    }
}



