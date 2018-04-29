#include "i2c_drive.h"
#include "../main.h"

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

void sendI2C(uint8_t* packet, uint8_t num_of_args, uint8_t slave_addr)
{
    int i;
    // Tell the master module what address it will place on the bus when
    // communicating with the slave.
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false);

    //put data to be sent into FIFO
    I2CMasterDataPut(I2C0_BASE, packet[0]);

    //if there is only one argument, we only need to use the
    //single send I2C function
    if(num_of_args == 1)
    {
        //Initiate send of data from the MCU
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C0_BASE));
    }

    //otherwise, we start transmission of multiple bytes on the
    //I2C bus
    else
    {
        //Initiate send of data from the MCU
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);

        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C0_BASE));

        //send num_of_args-2 pieces of data, using the
        //BURST_SEND_CONT command of the I2C module
        for(i = 1; i < (num_of_args - 1); i++)
        {
            //put next piece of data into I2C FIFO
            I2CMasterDataPut(I2C0_BASE, packet[i]);
            //send next data that was just placed into FIFO
            I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);

            // Wait until MCU is done transferring.
            while(I2CMasterBusy(I2C0_BASE));
        }

        //put last piece of data into I2C FIFO
        I2CMasterDataPut(I2C0_BASE, packet[num_of_args - 1]);
        //send next data that was just placed into FIFO
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C0_BASE));
    }
}


uint8_t  get_data_from_pressure(uint8_t reg, uint8_t slave_addr)
{
    //uint8_t i = 0;

    //specify that we are writing (a register address) to the
    //slave device
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false);

    //specify register to be read
    I2CMasterDataPut(I2C0_BASE, reg);

    //send control byte and register address byte to slave device
    I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_BURST_SEND_START);

    while(!I2CMasterBusy(I2C0_BASE));
    while(I2CMasterBusy(I2C0_BASE));

    //specify that we are going to read from slave device
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, true);

    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
    //wait for MCU to finish transaction
    while(!I2CMasterBusy(I2C0_BASE));
    while(I2CMasterBusy(I2C0_BASE));

    //return data pulled from the specified register
    return I2CMasterDataGet(I2C0_BASE);
}

uint8_t get_data_from_uv(uint8_t slave_addr)
{
    //specify that we are going to read from slave device
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, true);

    //send control byte and read from the register we
    //specified
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

    //wait for MCU to finish transaction
    while(I2CMasterBusy(I2C0_BASE));

    //return data pulled from the specified register
    return I2CMasterDataGet(I2C0_BASE);
}



