/*
 * I2C3.c
 *
 *  Created on: Nov 28, 2015
 *      Author: cli
 */



#include "tivaware_i2c.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"


void i2c3_init(){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C3);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C3);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    GPIOPinConfigure(GPIO_PD0_I2C3SCL);
    GPIOPinConfigure(GPIO_PD1_I2C3SDA);

    GPIOPinTypeI2CSCL(GPIO_PORTD_BASE, GPIO_PIN_0);
    GPIOPinTypeI2C(GPIO_PORTD_BASE, GPIO_PIN_1);

    I2CMasterInitExpClk(I2C3_BASE, SysCtlClockGet(), true);

}

void i2c3_read_byte(uint8_t addr, uint8_t reg_addr, uint8_t *ret){
	//set address
	I2CMasterSlaveAddrSet(I2C3_BASE, addr, 0);

	//send reg address
	I2CMasterDataPut(I2C3_BASE, reg_addr);
	I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);
	while(I2CMasterBusy(I2C3_BASE));

	//start the reading sequence
	I2CMasterSlaveAddrSet(I2C3_BASE, addr, 1);

	//send reg address to read
	I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
	while(I2CMasterBusy(I2C3_BASE));
	*ret = (uint8_t) I2CMasterDataGet(I2C3_BASE);
}

//should only be used for 2 bytes or more
void i2c3_read_bytes(uint8_t addr, uint8_t reg_addr, uint8_t *buffer, uint8_t length){

	uint8_t i;

	//set address
	I2CMasterSlaveAddrSet(I2C3_BASE, addr, 0);

	//send reg address
	I2CMasterDataPut(I2C3_BASE, reg_addr);
	I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);
	while(I2CMasterBusy(I2C3_BASE));

	//start the reading sequence
	I2CMasterSlaveAddrSet(I2C3_BASE, addr, 1);

	//send reg address to read
	I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
	while(I2CMasterBusy(I2C3_BASE));
	buffer[0] = (uint8_t) I2CMasterDataGet(I2C3_BASE);

	for (i = 1; i < length - 1; ++i){
		I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
		while(I2CMasterBusy(I2C3_BASE));
		buffer[i] = (uint8_t) I2CMasterDataGet(I2C3_BASE);
	}

	I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
	while(I2CMasterBusy(I2C3_BASE));
	buffer[length - 1] = (uint8_t) I2CMasterDataGet(I2C3_BASE);

}

void i2c3_write_byte(uint8_t addr, uint8_t reg_addr, uint8_t data){

	//set address
	I2CMasterSlaveAddrSet(I2C3_BASE, addr, 0);

	//send reg address
	I2CMasterDataPut(I2C3_BASE, reg_addr);
	I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);
	while(I2CMasterBusy(I2C3_BASE));

	//send data
	I2CMasterDataPut(I2C3_BASE, data);
	I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
	while(I2CMasterBusy(I2C3_BASE));
}

uint32_t i2c_check_err(){
	return I2CMasterErr(I2C3_BASE);
}
