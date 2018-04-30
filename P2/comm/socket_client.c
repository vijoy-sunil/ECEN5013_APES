/*
 * socket_client.c
 *
 *  Created on: Apr 27, 2018
 *      Author: VIJOY-PC
 */

#include "../main.h"
#include "socket_client.h"
#include "FreeRTOS.h" //should be the first to be included from amongst all free rtos files
#include <FreeRTOS_IP.h>
#include <FreeRTOS_Sockets.h>

#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/sysctl.h"

extern QueueHandle_t comm_socket_Queue;
client_packet_t* comm_client_packet;

void socket_client_task(void *pvParameters)
{
    //client_packet_t* comm_client_packet = (client_packet_t*)malloc(sizeof(client_packet_t));

    uint32_t ReceivedValue;
    Socket_t xClientSocket;
    socklen_t xSize = sizeof(struct freertos_sockaddr);
    static const TickType_t xTimeOut = pdMS_TO_TICKS(30000);//30 seconds

    /* Attempt to open the socket. */
    xClientSocket = FreeRTOS_socket(FREERTOS_AF_INET,
                                   FREERTOS_SOCK_STREAM,
                                   FREERTOS_IPPROTO_TCP);
    /* Check the socket was created. */
    configASSERT(xClientSocket != FREERTOS_INVALID_SOCKET);
    /* Set receive timeout*/
    BaseType_t ret = FreeRTOS_setsockopt(xClientSocket, 0,
                                        FREERTOS_SO_RCVTIMEO,
                                        &xTimeOut, sizeof(xTimeOut));
    configASSERT(ret == 0);        //0 for success
    /* Set Transmit timeout*/
    ret = FreeRTOS_setsockopt(xClientSocket, 0,
                             FREERTOS_SO_SNDTIMEO,
                             &xTimeOut, sizeof(xTimeOut));
    configASSERT(ret == 0);        //0 for success

    /* Bind the socket, but pass in NULL to let FreeRTOS+TCP choose the port num */
    ret = FreeRTOS_bind(xClientSocket, NULL, xSize);

    configASSERT(ret == 0);        //0 for success
    UARTprintf("Client created and bound to port\n");
    //set up the remote server address here
    struct freertos_sockaddr xRemoteAddress;
    /* Set the IP address & port of server to which client will transmit. */
    xRemoteAddress.sin_port = FreeRTOS_htons(PORT);

    #if (ipconfigUSE_DHCP == 0 )
       xRemoteAddress.sin_addr = FreeRTOS_inet_addr_quick(10,10,10,200);//static address
    #else
       xRemoteAddress.sin_addr = FreeRTOS_inet_addr_quick(192,168,0,40);//gets assigned by the dhcp on routerFreeRTOS_inet_addr_quick(192,168,1,13)
    #endif

    ret = FreeRTOS_connect(xClientSocket, &xRemoteAddress, sizeof(xRemoteAddress));

    if(ret!=0)
    {
       UARTprintf("Client could not connect to server:%d\n",ret);
       /* Initiate graceful shutdown. */
       FreeRTOS_shutdown(xClientSocket, FREERTOS_SHUT_RDWR);
       /* Wait for the socket to disconnect gracefully (indicated by FreeRTOS_recv()
          returning a FREERTOS_EINVAL error) before closing the socket. */
    }

    else
    {
        UARTprintf("Client connected to server\n");
        BaseType_t xBytesSent;
        while (1)
        {
            // block on notify from comm task
            xTaskNotifyWait(0xFFFFFFFF,
                            ReceivedValue,
                            &ReceivedValue,
                            portMAX_DELAY);

            if(ReceivedValue & SOCKET_WAKE_UP)
            {
                ReceivedValue = 0;
                //dummy tiva packet
                //comm_client_packet->clientInfo = 1;
                //comm_client_packet->clientStatus = 5;
                //comm_client_packet->uv_payLoad = 0.01;
                //comm_client_packet->pr_payLoad = 0.02;

                // send client pack struct to server
                xBytesSent = FreeRTOS_send(xClientSocket, /* The socket being sent to. */
                                           (void*) (comm_client_packet),/* The data being sent. */
                                           sizeof(client_packet_t),/* The remaining length of data to send. */
                                           0); /* ulFlags. */
                if (xBytesSent >= 0)
                {
                    //UARTprintf("TIVA client packet sent, Bytes :%d\n", xBytesSent);
                }

                else{
                    UARTprintf("Unable to send TIVA client packet\n");
                    break;
                }
            }
        }
    }

    while (1)
    {
    }
}


