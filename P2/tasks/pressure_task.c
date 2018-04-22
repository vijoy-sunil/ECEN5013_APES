#include <comms/i2c_drive.h>
#include "../main.h"
#include "./tasks/pressure_task.h"
#include "./utils/pack_msg.h"
#include "./tasks/comm_task.h"
#include "./comms/i2c_drive.h"

QueueHandle_t pressure_comm_Queue = NULL;
extern TaskHandle_t mainTask_handle;


void pressure_task(void *pvParameters)
{
    //BaseType_t ret;

    //data transfer queue
    pressure_comm_Queue = xQueueCreate( PRESSURE_COMM_QUEUE_LENGTH, sizeof(msg_pack_t));

    uint32_t pressure_ReceivedValue, signal;
    uint8_t pressure_data[3], packet[2];

    msg_pack_t* pressure_packet = (msg_pack_t*)malloc(sizeof(msg_pack_t));

    //config press sensor
    // Select control register(0x26)
    // Active mode, OSR = 128, barometer mode(0x39)
    packet[0] = PRESSURE_SLAVE_CONFIG_CNTRL;
    packet[1] = PRESSURE_SLAVE_BAR;
    sendI2C(packet, 2, PRESSURE_SLAVE_ADDR);


    while(1)
    {
        xTaskNotifyWait(0xFFFFFFFF,
                        pressure_ReceivedValue,
                        &pressure_ReceivedValue,
                        portMAX_DELAY);

#ifdef PRESSURE_ON

        pressure_data[0] = get_data_from_pressure(MSB_P, PRESSURE_SLAVE_ADDR);

        pressure_data[1] = get_data_from_pressure(CSB_P, PRESSURE_SLAVE_ADDR);

        pressure_data[2] = get_data_from_pressure(LSB_P, PRESSURE_SLAVE_ADDR);

        // Convert the data
        int pres = ((pressure_data[0] * 65536) + (pressure_data[1] * 256 + (pressure_data[2] & 0xF0)))/16;
        float pressure = (pres / 4.0) / 1000.0;

        //pack data
        pressure_packet->source = SENSOR_PRESSURE;
        pressure_packet->payload = pressure;

        //UARTprintf("msb: %x, csb: %x, lsb: %x\n", pressure_data[0], pressure_data[1], pressure_data[2]);
#endif

#ifndef PRESSURE_ON
        //pack data
        pressure_packet->source = SENSOR_PRESSURE;
        pressure_packet->payload = 0.02;
#endif

        //send data to comm task
        xQueueSend( pressure_comm_Queue, &pressure_packet, 0U);

        //send heartbeat to maintask
        if(pressure_ReceivedValue & PRESSURE_REQUEST_HB){
            pressure_ReceivedValue = 0;
            signal = PRESSURE_SEND_HB;
            xTaskNotify(mainTask_handle, signal, eSetBits);
        }
    }
}



