#include "../main.h"
#include "./tasks/comm_task.h"
#include "./utils/pack_msg.h"

//return string
char* get_client_info(char* client_info){
    char id[50];
    sprintf(id, "%d", CLIENT_ID);

    client_info = id;
    return client_info;
}

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
    uint32_t comm_ReceivedValue, signal;
    char* client_info = NULL;
    client_info = get_client_info(client_info);

    char line1[50], line2[200], line3[200], line4[200];
    char uv_alive[1], pr_alive[1], cm_alive[1];
    char uv_dead[1], pr_dead[1], cm_dead[1];

    task_status_t tstatus;
    while(1)
    {
        //functions
        //send heartbeat periodically

        xTaskNotifyWait(0xFFFFFFFF,
                        comm_ReceivedValue,
                        &comm_ReceivedValue,
                        portMAX_DELAY);

        //receive sensor data
        xQueueReceive( uv_comm_Queue, &packReceived, 0U );

        sprintf(line1, "CLIENT_ID: %d\n" ,client_info);
        //pack data as string
        if(packReceived->source == SENSOR_UV)
        {
            sprintf(line3, "SENSOR DATA: [uv] | [%f] | [?]\n",packReceived->payload);
        }

        xQueueReceive( pressure_comm_Queue, &packReceived, 0U );
        if(packReceived->source == SENSOR_PRESSURE)
        {
            sprintf(line4, "SENSOR DATA: [pressure] | [%f] | [?]\n",packReceived->payload);
        }

        uv_alive[0] = ' ';
        pr_alive[0] = ' ';
        cm_alive[0] = ' ';
        uv_dead[0] = ' ';
        pr_dead[0] = ' ';
        cm_dead[0] = ' ';

        //get task status from main task
        //queue receive here
        xQueueReceive( main_comm_Queue, &tstatus, 0U );

        if(tstatus & UV_ALIVE)
            sprintf(uv_alive, "%c", 'U');

        else if(tstatus & UV_DEAD)
            sprintf(uv_dead, "%c", 'U');

        if(tstatus & PRESSURE_ALIVE)
            sprintf(pr_alive, "%c", 'P');

        else if(tstatus & PRESSURE_DEAD)
            sprintf(pr_dead, "%c", 'P');

        if(tstatus & COMM_ALIVE)
            sprintf(cm_alive, "%c", 'C');

        else if(tstatus & COMM_DEAD)
            sprintf(cm_dead, "%c", 'C');

        sprintf(line2, "CLIENT_STATUS: alive [%c %c %c] | dead [%c %c %c]\n" ,uv_alive[0], pr_alive[0], cm_alive[0],
                                                                                 uv_dead[0], pr_dead[0], cm_dead[0]);


        // client info:
        client_packet->clientInfo = line1;
        // client status: alive tasks [] | dead tasks [] |
        client_packet->clientStatus = line2;
        // sensor source | payload | unit | for uv sensor
        client_packet->payLoad1 = line3;
        // sensor source | payload | unit | for pressure sensor
        client_packet->payLoad2 = line4;

#ifdef SERIAL_DEBUG
        //print string in terminal
        UARTprintf("%s", line1);
        UARTprintf("%s", line2);
        UARTprintf("%s", line3);
        UARTprintf("%s", line4);
#endif

        //send to bbg

        //send heart beat
        if(comm_ReceivedValue & COMM_REQUEST_HB){
            comm_ReceivedValue = 0;
            signal = COMM_SEND_HB;
            xTaskNotify(mainTask_handle, signal, eSetBits);
            //UARTprintf("COMM task send hb\r\n");
        }

    }
}



