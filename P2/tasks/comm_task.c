#include "../main.h"
#include "./tasks/comm_task.h"


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

void comm_task(void *pvParameters)
{
    BaseType_t ret;

    uint32_t comm_ReceivedValue, signal;
    while(1)
    {
        //UARTprintf("COMM running\r\n");
        //functions
        //send heartbeat periodically

        xTaskNotifyWait(0xFFFFFFFF,
                        comm_ReceivedValue,
                        &comm_ReceivedValue,
                        portMAX_DELAY);

        //pack sensor data and send
        //while(1);

        if(comm_ReceivedValue & COMM_REQUEST_HB){
            comm_ReceivedValue = 0;
            signal = COMM_SEND_HB;
            xTaskNotify(mainTask_handle, signal, eSetBits);
            //UARTprintf("COMM task send hb\r\n");
        }

    }
}



