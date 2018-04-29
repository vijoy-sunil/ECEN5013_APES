/*
 * NetworkInterface.h
 *
 *  Created on: Apr 25, 2018
 *      Author: VIJOY-PC
 */

#ifndef FREERTOS_TCP_PORTABLE_NETWORKINTERFACE_NETWORKINTERFACE_H_
#define FREERTOS_TCP_PORTABLE_NETWORKINTERFACE_NETWORKINTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_nvic.h"
#include "inc/hw_emac.h"
#include "inc/hw_flash.h"

#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/flash.h"

// FreeRTOS
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "emac.h"
#include "NetworkBufferManagement.h"

extern uint32_t output_clock_rate_hz;
//*****************************************************************************
// Ethernet DMA descriptors.
//
// The MAC hardware needs a minimum of 10 receive descriptors to operate. The
// number used will be application-dependent and should be tuned for best
// performance.
//*****************************************************************************
#define NUM_TX_DESCRIPTORS 10
#define NUM_RX_DESCRIPTORS 10
tEMACDMADescriptor g_psRxDescriptor[NUM_TX_DESCRIPTORS];
tEMACDMADescriptor g_psTxDescriptor[NUM_RX_DESCRIPTORS];
uint32_t g_ui32RxDescIndex;
uint32_t g_ui32TxDescIndex;
//*****************************************************************************
//
// Transmit and receive buffers. These will typically be allocated within your
// network stack somewhere.
//
//*****************************************************************************
#define RX_BUFFER_SIZE 1536
uint8_t g_ppui8RxBuffer[NUM_RX_DESCRIPTORS][RX_BUFFER_SIZE];

void ProcessReceivedPacket(void);
void EthernetIntHandler(void);
static void prvEMACDeferredInterruptHandlerTask( void *pvParameters );
static int32_t PacketTransmit(uint8_t *pui8Buf, int32_t i32BufLen);
void InitDescriptors(uint32_t ui32Base);
void MACinit(void);
BaseType_t xNetworkInterfaceInitialise( void );
BaseType_t xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxDescriptor,
                                    BaseType_t xReleaseAfterSend );
void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent );

#endif /* FREERTOS_TCP_PORTABLE_NETWORKINTERFACE_NETWORKINTERFACE_H_ */
