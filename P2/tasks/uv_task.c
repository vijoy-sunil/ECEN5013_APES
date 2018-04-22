#include <comms/i2c_drive.h>
#include "../main.h"
#include "./tasks/uv_task.h"
#include "./utils/pack_msg.h"
#include "./tasks/comm_task.h"
#include "./comms/i2c_drive.h"


QueueHandle_t uv_comm_Queue = NULL;
extern TaskHandle_t mainTask_handle;

int initialize_uv(void){
    int ret = 0;
    uint8_t packet[1];
    // clear ACK bit
    packet[0] = IT_1T  | UV_CMD_WDM;
    sendI2C(packet, 1, UV_SLAVE_ADDR);

    //write 0x06 into 0x70 address
    packet[0] = UV_INIT_BYTE;
    sendI2C(packet, 1,  UV_SLAVE_ADDR);

    ret = 1;
    return ret;
}

void uv_task(void *pvParameters)
{
    //data transfer queue
    uv_comm_Queue = xQueueCreate( UV_COMM_QUEUE_LENGTH, sizeof(msg_pack_t) );
    int uv_rad;
    uv_index_t uv_index;
    uint32_t uv_ReceivedValue, signal;
    msg_pack_t* uv_packet = (msg_pack_t*)malloc(sizeof(msg_pack_t));

#ifdef UV_ON
    uint8_t uv_data[2];
    //initialize uv sensor here
    int ret = initialize_uv();
    if(ret)
        UARTprintf("UV sensor init SUCCESS\n");
    else
        UARTprintf("UV sensor init ERROR");
#endif


    while(1)
    {
        //send heartbeat periodically
        xTaskNotifyWait(0xFFFFFFFF,
                        uv_ReceivedValue,
                        &uv_ReceivedValue,
                        portMAX_DELAY);


        //get sensor data 2 * 8 bit
#ifdef UV_ON
        uv_data[0] = get_data_from_uv(UV_SLAVE_MSB);

        uv_data[1] = get_data_from_uv(UV_SLAVE_LSB);
        uv_rad = (uv_data[1] << 8 |  uv_data[0]);

        //https://www.vishay.com/docs/84310/designingveml6070.pdf
        if(uv_rad >= 0 && uv_rad <= 560)
            uv_index = LOW;
        else if(uv_rad >= 561 && uv_rad <= 1120)
            uv_index = MODERATE;
        else if(uv_rad >= 1121 && uv_rad <= 1494)
            uv_index = HIGH;
        else if(uv_rad >= 1495 && uv_rad <= 2054)
            uv_index = VERY_HIGH;
        else
            uv_index = EXTREME;

        //pack data
        uv_packet->source = SENSOR_UV;
        uv_packet->payload = uv_index;

        //UARTprintf("msb: %d, lsb: %d\n", uv_data[0], uv_data[1]);
#endif

#ifndef UV_ON
        //pack data
        uv_packet->source = SENSOR_UV;
        uv_packet->payload = 0.01;
#endif

        //send data to comm task
        xQueueSend( uv_comm_Queue, &uv_packet, 0U);

        //send heatbeat to maintask
        if(uv_ReceivedValue & UV_REQUEST_HB){
            uv_ReceivedValue = 0;
            signal = UV_SEND_HB;
            xTaskNotify(mainTask_handle, signal, eSetBits);
        }
    }
}
