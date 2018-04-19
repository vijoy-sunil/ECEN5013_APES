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
    pressure_comm_Queue = xQueueCreate( PRESSURE_COMM_QUEUE_LENGTH, sizeof(unsigned char));

    uint32_t pressure_ReceivedValue, signal;
    uint8_t pressure_data[4];

    msg_pack_t* pressure_packet = (msg_pack_t*)malloc(sizeof(msg_pack_t));
    uint32_t packet[4];
    while(1)
    {
        //functions
        //send heartbeat periodically
        xTaskNotifyWait(0xFFFFFFFF,
                        pressure_ReceivedValue,
                        &pressure_ReceivedValue,
                        portMAX_DELAY);

        // Select control register(0x26)
        // Active mode, OSR = 128, barometer mode(0x39)
        packet[0] = PRESSURE_SLAVE_CONFIG_CNTRL1;
        packet[1] = PRESSURE_SLAVE_CONFIG_CNTRL2;
        sendI2C(packet, 2, PRESSURE_SLAVE_ADDR);

        // Read 4 bytes of data from register(0x00)
        // status, pres msb1, pres msb, pres lsb
        packet[0] = PRESSURE_SLAVE_DATA_ADDR;
        sendI2C(packet, 1, PRESSURE_SLAVE_ADDR);

        receiveI2C(packet, 4, PRESSURE_SLAVE_ADDR);
        pressure_data[0] = (uint8_t)(packet[0]);
        pressure_data[1] = (uint8_t)(packet[1]);
        pressure_data[2] = (uint8_t)(packet[2]);
        pressure_data[3] = (uint8_t)(packet[3]);

        // Convert the data to 20-bits
        int pres = ((pressure_data[1] * 65536) + (pressure_data[2] * 256 + (pressure_data[3] & 0xF0))) / 16;
        float pressure = (pres / 4.0) / 1000.0;

        //pack data
        pressure_packet->source = SENSOR_PRESSURE;
        pressure_packet->payload = pressure;

        //send data to comm task
        xQueueSend( pressure_comm_Queue, pressure_packet, portMAX_DELAY);


        if(pressure_ReceivedValue & PRESSURE_REQUEST_HB){
            pressure_ReceivedValue = 0;
            signal = PRESSURE_SEND_HB;
            xTaskNotify(mainTask_handle, signal, eSetBits);
            //UARTprintf("PRESSURE task send hb\r\n");
        }


    }
}



