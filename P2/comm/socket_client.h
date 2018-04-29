/*
 * socket_client.h
 *
 *  Created on: Apr 27, 2018
 *      Author: VIJOY-PC
 */
void socket_client_task(void *pvParameters);
#define PORT    5000

typedef struct{
    uint32_t clientInfo;
    uint32_t clientStatus;

    float uv_payLoad;
    float pr_payLoad;
}client_packet_t;


