#include "./comms/i2c_drive.h"
#include "../main.h"

#define NUM_I2C_DATA 3

#define SLAVE_ADDRESS 0x3C

extern uint32_t output_clock_rate_hz;

void ConfigureI2C(void)
{
    // The I2C0 peripheral must be enabled before use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

    //GPIO port B needs to be enabled so these pins can
    // be used.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Configure the pin muxing for I2C0 functions on port B2 and B3.
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    // Select the I2C function for these pins.  This function will also
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    // Enable and initialize the I2C0 master module.  Use the system clock for
    // the I2C0 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.  For this example we will use a data rate of 100kbps.
    I2CMasterInitExpClk(I2C0_BASE, output_clock_rate_hz, false);
}

void sendI2C(uint32_t* packet, uint32_t packet_size, uint8_t slave_addr)
{
    // Set the slave address
    I2CSlaveInit(I2C0_BASE, slave_addr);

    // Tell the master module what address it will place on the bus when
    // communicating with the slave.  Set the address to SLAVE_ADDRESS
    // (as set in the slave module).  The receive parameter is set to false
    // which indicates the I2C Master is initiating a writes to the slave.
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false);

    //uint32_t pui32DataTx[packet_size];
    uint32_t ui32Index;

    // Send 3 peices of I2C data from the master to the slave.
    for(ui32Index = 0; ui32Index < packet_size; ui32Index++)
    {
        // Place the data to be sent in the data register
        I2CMasterDataPut(I2C0_BASE, packet[ui32Index]);

        // Initiate send of data from the master
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

        // Wait until the slave has received and acknowledged the data.
        while(!(I2CSlaveStatus(I2C0_BASE) & I2C_SLAVE_ACT_RREQ));
    }
}

void  receiveI2C(uint32_t* packet, uint32_t packet_size, uint8_t slave_addr)
{
    // Set the slave address to SLAVE_ADDRESS
    I2CSlaveInit(I2C0_BASE, slave_addr);

    // Tell the master module what address it will place on the bus when
    // communicating with the slave.  Set the address to SLAVE_ADDRESS
    // (as set in the slave module).
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, true);

    uint32_t ui32Index;

    // Reset receive buffer.
    for(ui32Index = 0; ui32Index < packet_size; ui32Index++)
        packet[ui32Index] = 0;

    for(ui32Index = 0; ui32Index < packet_size; ui32Index++)
    {
        // Tell the master to read data.
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

        // Wait until transcation is done
        while(I2CMasterBusy(I2C0_BASE));

        // Read the data from the master.
        packet[ui32Index] = I2CMasterDataGet(I2C0_BASE);
    }
}

void get_uvdata(uint8_t* result){

}




