#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

// 2 Hz blink rate (500 msec)
#define BLINK_DELAY ((SysCtlClockGet()/3) * 8.33)

// System clock rate in Hz.
uint32_t g_ui32SysClock;


// The error routine that is called if the driver library encounters an error.
#ifdef DEBUG
void __error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


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
    UARTStdioConfig(0, 115200, g_ui32SysClock);
}


int main(void)
{
    uint32_t blink_cnt = 0;
    // Run from the PLL at 120 MHz.
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                SYSCTL_CFG_VCO_480), 120000000);

    // Enable the GPIO pins for the LED D1 (PN1).
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    // Initialize the UART.
    ConfigureUART();

    UARTprintf("Project for: VIJOY SUNIL KUMAR 04-05-2018 \r\n");

    // main loop
    while(1)
    {
        ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
        // delay
        ROM_SysCtlDelay(BLINK_DELAY);
        ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
        //delay
        ROM_SysCtlDelay(BLINK_DELAY);

        blink_cnt++;
        UARTprintf("LED blink count: %d\r\n", blink_cnt);

    }

}
