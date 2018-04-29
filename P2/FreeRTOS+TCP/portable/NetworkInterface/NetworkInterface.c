#include "NetworkInterface.h"
#include "../../../comm/socket_client.h"
#include "../../../main.h"

TaskHandle_t EMACtask_handle;
TaskHandle_t socket_clientTask_handle;
extern int socket_client_task_ready;


//*****************************************************************************
// Read a packet from the DMA receive buffer and return the number of bytes
// read.
//
//*****************************************************************************
void ProcessReceivedPacket(void)
{
    // Make sure that we own the receive descriptor.
    if(!(g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus & DES0_RX_CTRL_OWN))
    {
        //
        // We own the receive descriptor so check to see if it contains a valid
        // frame.
        //
        if(!(g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus &
        DES0_RX_STAT_ERR))
        {
            //
            // We have a valid frame. First check that the "last descriptor"
            // flag is set. We sized the receive buffer such that it can
            // always hold a valid frame so this flag should never be clear at
            // this point but...
            //
            if(g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus & DES0_RX_STAT_LAST_DESC)
            {
                // notify deferred handler
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                vTaskNotifyGiveFromISR(EMACtask_handle, &xHigherPriorityTaskWoken);
            }
        }
    }
}
//*****************************************************************************
//
// The interrupt handler for the Ethernet interrupt.
//
//*****************************************************************************
void EthernetIntHandler(void)
{
    uint32_t ui32Temp;

    // Read and Clear the interrupt.
    ui32Temp = EMACIntStatus(EMAC0_BASE, true);
    EMACIntClear(EMAC0_BASE, ui32Temp);

    // Check to see if an RX Interrupt has occurred.
    if(ui32Temp & EMAC_INT_RECEIVE)
    {
        // Indicate that a packet has been received.
        ProcessReceivedPacket();
    }
}

//********************************************************************************
// The deferred interrupt handler is a standard RTOS task.  FreeRTOS's centralised
// deferred interrupt handling capabilities can also be used.
//********************************************************************************
static void prvEMACDeferredInterruptHandlerTask( void *pvParameters )
{
    NetworkBufferDescriptor_t *pxBufferDescriptor;
    size_t xBytesReceived;
    // Used to indicate that xSendEventStructToIPTask() is being called because
    // of an Ethernet receive event.
    IPStackEvent_t xRxEvent;

    for( ;; )
    {
        /*Wait for the Ethernet MAC interrupt to indicate that another packet
        has been received.  The task notification is used in a similar way to a
        counting semaphore to count Rx events, but is a lot more efficient than
        a semaphore.*/
        ulTaskNotifyTake( pdFALSE, portMAX_DELAY );

        /* See how much data was received.  Here it is assumed ReceiveSize() is
        a peripheral driver function that returns the number of bytes in the
        received Ethernet frame.*/

        xBytesReceived =  ((g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus &
                DES0_RX_STAT_FRAME_LENGTH_M) >>
                DES0_RX_STAT_FRAME_LENGTH_S);

        if( xBytesReceived > 0 )
        {
            /* Allocate a network buffer descriptor that points to a buffer
            large enough to hold the received frame.  As this is the simple
            rather than efficient example the received data will just be copied
            into this buffer.*/
            pxBufferDescriptor = pxGetNetworkBufferWithDescriptor( xBytesReceived, 0 );

            if( pxBufferDescriptor != NULL )
            {
                /* pxBufferDescriptor->pucEthernetBuffer now points to an Ethernet
                buffer large enough to hold the received data.  Copy the
                received data into pcNetworkBuffer->pucEthernetBuffer.  Here it
                is assumed ReceiveData() is a peripheral driver function that
                copies the received data into a buffer passed in as the function's
                parameter.  Remember! While is is a simple robust technique -
                it is not efficient.  An example that uses a zero copy technique
                is provided further down this page.*/

                //memcpy here
                memcpy((void*)pxBufferDescriptor->pucEthernetBuffer, (void*)g_psRxDescriptor[g_ui32RxDescIndex].pvBuffer1, pxBufferDescriptor->xDataLength);

                //update index
                g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus =
                        DES0_RX_CTRL_OWN;

                g_ui32RxDescIndex++;

                if (g_ui32RxDescIndex == NUM_RX_DESCRIPTORS)
                {
                    g_ui32RxDescIndex = 0;
                }

                /* See if the data contained in the received Ethernet frame needs
                to be processed.  NOTE! It is preferable to do this in
                the interrupt service routine itself, which would remove the need
                to unblock this task for packets that don't need processing.*/
                if( eConsiderFrameForProcessing( pxBufferDescriptor->pucEthernetBuffer ) == eProcessBuffer )
                {
                    // The event about to be sent to the TCP/IP is an Rx event.
                    xRxEvent.eEventType = eNetworkRxEvent;

                    // pvData is used to point to the network buffer descriptor that
                    // now references the received data.
                    xRxEvent.pvData = ( void * ) pxBufferDescriptor;

                    // Send the data to the TCP/IP stack.
                    if( xSendEventStructToIPTask( &xRxEvent, 0 ) == pdFALSE )
                    {
                        // The buffer could not be sent to the IP task so the buffer
                        // must be released.
                        vReleaseNetworkBufferAndDescriptor( pxBufferDescriptor );

                        // Make a call to the standard trace macro to log the
                        // occurrence.
                        iptraceETHERNET_RX_EVENT_LOST();
                    }
                    else
                    {
                        // The message was successfully sent to the TCP/IP stack.
                        //Call the standard trace macro to log the occurrence.
                        iptraceNETWORK_INTERFACE_RECEIVE();
                    }
                }
                else
                {
                    // The Ethernet frame can be dropped, but the Ethernet buffer
                    // must be released.
                    vReleaseNetworkBufferAndDescriptor( pxBufferDescriptor );
                }
            }
            else
            {
                // The event was lost because a network buffer was not available.
                // Call the standard trace macro to log the occurrence.
                iptraceETHERNET_RX_EVENT_LOST();
            }
        }
    }
}


//*****************************************************************************
//
// Transmit a packet from the supplied buffer. This function would be called
// directly by the application. pui8Buf points to the Ethernet frame to send
// and i32BufLen contains the number of bytes in the frame.
//
//*****************************************************************************
static int32_t PacketTransmit(uint8_t *pui8Buf, int32_t i32BufLen)
{
    // Wait for the transmit descriptor to free up.
    while(g_psTxDescriptor[g_ui32TxDescIndex].ui32CtrlStatus & DES0_TX_CTRL_OWN)
    {
        // Spin and waste time.
    }

    // Move to the next descriptor.
    g_ui32TxDescIndex++;
    if(g_ui32TxDescIndex == NUM_TX_DESCRIPTORS)
    {
        g_ui32TxDescIndex = 0;
    }

    // Fill in the packet size and pointer, and tell the transmitter to start
    // work.
    g_psTxDescriptor[g_ui32TxDescIndex].ui32Count = (uint32_t)i32BufLen;
    g_psTxDescriptor[g_ui32TxDescIndex].pvBuffer1 = pui8Buf;
    g_psTxDescriptor[g_ui32TxDescIndex].ui32CtrlStatus = (DES0_TX_CTRL_LAST_SEG | DES0_TX_CTRL_FIRST_SEG |
            DES0_TX_CTRL_INTERRUPT | DES0_TX_CTRL_IP_ALL_CKHSUMS | DES0_TX_CTRL_CHAINED | DES0_TX_CTRL_OWN);

    // Tell the DMA to reacquire the descriptor now that we�ve filled it in.
    // This call is benign if the transmitter hasn�t stalled and checking
    // the state takes longer than just issuing a poll demand so we do this
    // for all packets.
    EMACTxDMAPollDemand(EMAC0_BASE);

    // Return the number of bytes sent.
    return(i32BufLen);
}
//*****************************************************************************
//
// Initialize the transmit and receive DMA descriptors.
//
//*****************************************************************************
void InitDescriptors(uint32_t ui32Base)
{
    uint32_t ui32Loop;
    //
    // Initialize each of the transmit descriptors. Note that we leave the
    // buffer pointer and size empty and the OWN bit clear here since we have
    // not set up any transmissions yet.
    //
    for(ui32Loop = 0; ui32Loop < NUM_TX_DESCRIPTORS; ui32Loop++)
    {
        g_psTxDescriptor[ui32Loop].ui32Count = DES1_TX_CTRL_SADDR_INSERT;
        g_psTxDescriptor[ui32Loop].DES3.pLink = (ui32Loop == (NUM_TX_DESCRIPTORS - 1)) ? g_psTxDescriptor : &g_psTxDescriptor[ui32Loop + 1];

        g_psTxDescriptor[ui32Loop].ui32CtrlStatus = (DES0_TX_CTRL_LAST_SEG | DES0_TX_CTRL_FIRST_SEG | DES0_TX_CTRL_INTERRUPT | DES0_TX_CTRL_CHAINED |
                DES0_TX_CTRL_IP_ALL_CKHSUMS);
    }
    //
    // Initialize each of the receive descriptors. We clear the OWN bit here
    // to make sure that the receiver doesn�t start writing anything
    // immediately.
    //
    for(ui32Loop = 0; ui32Loop < NUM_RX_DESCRIPTORS; ui32Loop++)
    {
        g_psRxDescriptor[ui32Loop].ui32CtrlStatus = 0;
        g_psRxDescriptor[ui32Loop].ui32Count = (DES1_RX_CTRL_CHAINED | (RX_BUFFER_SIZE << DES1_RX_CTRL_BUFF1_SIZE_S));
        g_psRxDescriptor[ui32Loop].pvBuffer1 = g_ppui8RxBuffer[ui32Loop];

        g_psRxDescriptor[ui32Loop].DES3.pLink = (ui32Loop == (NUM_RX_DESCRIPTORS - 1)) ? g_psRxDescriptor : &g_psRxDescriptor[ui32Loop + 1];
    }
    //
    // Set the descriptor pointers in the hardware.
    //
    EMACRxDMADescriptorListSet(ui32Base, g_psRxDescriptor);
    EMACTxDMADescriptorListSet(ui32Base, g_psTxDescriptor);
    //
    // Start from the beginning of both descriptor chains. We actually set
    // the transmit descriptor index to the last descriptor in the chain
    // since it will be incremented before use and this means the first
    // transmission we perform will use the correct descriptor.
    //
    g_ui32RxDescIndex = 0;
    g_ui32TxDescIndex = NUM_TX_DESCRIPTORS - 1;
}
//*****************************************************************************
//
// This example demonstrates the use of the Ethernet Controller.
//
//*****************************************************************************
void MACinit(void)
{
    uint32_t ui32User0, ui32User1, ui32Loop;
    uint8_t ui8PHYAddr, pui8MACAddr[6];

    // Read the MAC address from the user registers.
    FlashUserGet(&ui32User0, &ui32User1);
    if((ui32User0 == 0xffffffff) || (ui32User1 == 0xffffffff))
    {
        // We should never get here. This is an error if the MAC address has
        // not been programmed into the device. Exit the program.
        while(1)
        {
        }
    }

    // Convert the 24/24 split MAC address from NV ram into a 32/16 split MAC
    // address needed to program the hardware registers, then program the MAC
    // address into the Ethernet Controller registers.
    //
    pui8MACAddr[0] = ((ui32User0 >> 0) & 0xff);
    pui8MACAddr[1] = ((ui32User0 >> 8) & 0xff);
    pui8MACAddr[2] = ((ui32User0 >> 16) & 0xff);
    pui8MACAddr[3] = ((ui32User1 >> 0) & 0xff);
    pui8MACAddr[4] = ((ui32User1 >> 8) & 0xff);
    pui8MACAddr[5] = ((ui32User1 >> 16) & 0xff);

    // Enable and reset the Ethernet modules.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EMAC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EPHY0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_EMAC0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_EPHY0);

    //Wait for the MAC to be ready.
    //UARTprintf("Waiting for MAC to be ready\n");
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_EMAC0))
    {
    }

    //UARTprintf("MAC active\n");
    // Configure for use with the internal PHY.
    ui8PHYAddr = 0;
    EMACPHYConfigSet(EMAC0_BASE, (EMAC_PHY_TYPE_INTERNAL | EMAC_PHY_INT_MDIX_EN | EMAC_PHY_AN_100B_T_FULL_DUPLEX));

    // Reset the MAC to latch the PHY configuration.
    EMACReset(EMAC0_BASE);


    // Initialize the MAC and set the DMA mode.
    EMACInit(EMAC0_BASE, output_clock_rate_hz, EMAC_BCONFIG_MIXED_BURST | EMAC_BCONFIG_PRIORITY_FIXED, 4, 4, 0);

    // Set MAC configuration options.
    EMACConfigSet(EMAC0_BASE, (EMAC_CONFIG_FULL_DUPLEX | EMAC_CONFIG_CHECKSUM_OFFLOAD | EMAC_CONFIG_7BYTE_PREAMBLE |
            EMAC_CONFIG_IF_GAP_96BITS | EMAC_CONFIG_USE_MACADDR0 | EMAC_CONFIG_SA_FROM_DESCRIPTOR | EMAC_CONFIG_BO_LIMIT_1024),
                  (EMAC_MODE_RX_STORE_FORWARD | EMAC_MODE_TX_STORE_FORWARD | EMAC_MODE_TX_THRESHOLD_64_BYTES | EMAC_MODE_RX_THRESHOLD_64_BYTES), 0);

    // Initialize the Ethernet DMA descriptors.
    InitDescriptors(EMAC0_BASE);

    // Program the hardware with its MAC address (for filtering).
    EMACAddrSet(EMAC0_BASE, 0, pui8MACAddr);

    // Wait for the link to become active.
    //UARTprintf("Waiting for PHY to be active\n");
    while((EMACPHYRead(EMAC0_BASE, ui8PHYAddr, EPHY_BMSR) & EPHY_BMSR_LINKSTAT) == 0)
    {

    }

    //UARTprintf("Link active\n");
    // Set MAC filtering options. We receive all broadcast and multicast
    // packets along with those addressed specifically for us.
    EMACFrameFilterSet(EMAC0_BASE, (EMAC_FRMFILTER_SADDR | EMAC_FRMFILTER_PASS_MULTICAST | EMAC_FRMFILTER_PASS_NO_CTRL));

    // Clear any pending interrupts.
    EMACIntClear(EMAC0_BASE, EMACIntStatus(EMAC0_BASE, false));

    // Mark the receive descriptors as available to the DMA to start
    // the receive processing.
    for(ui32Loop = 0; ui32Loop < NUM_RX_DESCRIPTORS; ui32Loop++)
    {
        g_psRxDescriptor[ui32Loop].ui32CtrlStatus |= DES0_RX_CTRL_OWN;
    }

    // Enable the Ethernet MAC transmitter and receiver.
    EMACTxEnable(EMAC0_BASE);
    EMACRxEnable(EMAC0_BASE);

    // Enable the Ethernet interrupt.
    IntEnable(INT_EMAC0);

    // Enable the Ethernet RX Packet interrupt source.
    EMACIntEnable(EMAC0_BASE, EMAC_INT_RECEIVE);
}

//****************************************************************************************
// xNetworkInterfaceInitialise() must prepare the Ethernet MAC to send and receive data.
// In most cases this will just involve calling whichever initialise function is provided
// with the Ethernet MAC peripheral drivers - which will in turn ensure the MAC hardware
// is enabled and clocked, as well as configure the MAC peripheral's DMA descriptors.
//****************************************************************************************
BaseType_t xNetworkInterfaceInitialise( void )
{


    xTaskCreate( prvEMACDeferredInterruptHandlerTask, "EMAC", configMINIMAL_STACK_SIZE*10, NULL,
                 configMAX_PRIORITIES - 1, &EMACtask_handle );
    vTaskDelay(200);

    MACinit();

    return pdPASS;
}

//****************************************************************************************
// The network interface port layer must provide a function called xNetworkInterfaceOutput()
// that sends data received from the embedded TCP/IP stack to the Ethernet MAC driver for transmission.

// If xReleaseAfterSend does not equal pdFALSE then both the buffer and the buffer's descriptor must be
// released (returned) back to the embedded TCP/IP stack by the driver code when they are no longer required.
// If xReleaseAfterSend is pdFALSE then both the network buffer and the buffer's descriptor will be released
// by the TCP/IP stack itself (in which case the driver does not need to release them).
//****************************************************************************************
BaseType_t xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxDescriptor,
                                    BaseType_t xReleaseAfterSend )
{
    /* Simple network interfaces (as opposed to more efficient zero copy network
    interfaces) just use Ethernet peripheral driver library functions to copy
    data from the FreeRTOS+TCP buffer into the peripheral driver's own buffer.
    This example assumes SendData() is a peripheral driver library function that
    takes a pointer to the start of the data to be sent and the length of the
    data to be sent as two separate parameters.  The start of the data is located
    by pxDescriptor->pucEthernetBuffer.  The length of the data is located
    by pxDescriptor->xDataLength. */

    PacketTransmit( pxDescriptor->pucEthernetBuffer, pxDescriptor->xDataLength );

    /* Call the standard trace macro to log the send event. */
    iptraceNETWORK_INTERFACE_TRANSMIT();
    if( xReleaseAfterSend != pdFALSE )
    {
        vReleaseNetworkBufferAndDescriptor( pxDescriptor );
    }
    return pdTRUE;
}

//****************************************************************************************
// The tasks that use the network
// are created in the vApplicationIPNetworkEventHook() hook function
// below.  The hook function is called when the network connects.
//****************************************************************************************

void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
    static BaseType_t xTasksAlreadyCreated = pdFALSE;
    uint32_t ulIPAddress, ulNetMask, ulGatewayAddress, ulDNSServerAddress;
    //int8_t cBuffer[ 16 ];

    /* Both eNetworkUp and eNetworkDown events can be processed here. */
    if( eNetworkEvent == eNetworkUp )
    {
        //UARTprintf( "Network up\n");
        /* The network is up and configured.  Print out the configuration,
        which may have been obtained from a DHCP server. */
        FreeRTOS_GetAddressConfiguration( &ulIPAddress,
                                          &ulNetMask,
                                          &ulGatewayAddress,
                                          &ulDNSServerAddress );

        /* Convert the IP address to a string then print it out. */
        //FreeRTOS_inet_ntoa( ulIPAddress, cBuffer );
        //UARTprintf( "IP Address: %s\r\n", cBuffer );

        /* Convert the net mask to a string then print it out. */
        //FreeRTOS_inet_ntoa( ulNetMask, cBuffer );
        //UARTprintf( "Subnet Mask: %s\r\n", cBuffer );

        /* Convert the IP address of the gateway to a string then print it out. */
        //FreeRTOS_inet_ntoa( ulGatewayAddress, cBuffer );
        //UARTprintf( "Gateway IP Address: %s\r\n", cBuffer );

        /* Convert the IP address of the DNS server to a string then print it out. */
        //FreeRTOS_inet_ntoa( ulDNSServerAddress, cBuffer );
        //UARTprintf( "DNS server IP Address: %s\r\n", cBuffer );

        /* Create the tasks that use the TCP/IP stack if they have not already
        been created. */
        if( xTasksAlreadyCreated == pdFALSE )
        {
            /*
             * For convenience, tasks that use FreeRTOS+TCP can be created here
             * to ensure they are not created before the network is usable.
             */
            xTaskCreate(socket_client_task, "CLIENT-TASK",
                        configMINIMAL_STACK_SIZE, NULL, 4, &socket_clientTask_handle);

            xTasksAlreadyCreated = pdTRUE;
            //notify comm that socket task is ready
            socket_client_task_ready = 1;
        }
    }
}














