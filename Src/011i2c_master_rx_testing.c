/*
 * 011i2c_master_rx_testing.c
 *
 *  Created on: 21.03.2026
 *      Author: Joelikane
 */

/*
 * I2C Master (STM32F407xx) and I2C Slave (Arduino) communication.
 *
 * When button on the master is pressed, master should read and display data from Arduino slave connected.
 * First master has to get the length of the data from the slave to read subsequent data from the slave.
 *
 * 1. Use I2C SCL = 100KHz (standard mode)
 * 2: Use 2 pull-up resistors (of value 3.3KOhm or 4.7KOhm) or internal pull up resistors for SDA and SCL
 *  lines
 *
 *Procedure to read the data from the slave
 *
 *1. Master sends command code 0x51 to read the length (1 byte) of the data from the slave
 *2. Master send command 0x52 to read the complete data from the slave
 */


#include<stdio.h>
#include<string.h>
#include "stm32f407xx.h"

#define MY_ADDR			0x61

#define SLAVE_ADDR		0x68

void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i ++);
}

I2C_Handle_t I2C1Handle;

//rcv buffer
uint8_t rcv_buffer[32];


/*
 * PB6 -> SCL
 * PB9 -> SDA
 */

void I2C1_GPIOInits(void)
{
	GPIO_Handle_t I2CPins;

	I2CPins.pGPIOx = GPIOB;
	I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	I2CPins.GPIO_PinConfig.GPIO_PinaltFunMode = 4; //AF4
	I2CPins.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_OD; // Pin output type should be open drain for I2C
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU; //pull up activated
	I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;

	//SCL
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&I2CPins);

	//SDA
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_Init(&I2CPins);

}

void I2C1_Inits(void)
{
	I2C1Handle.pI2Cx = I2C1;
	I2C1Handle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
	I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDR;
	I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM; //Standard mode
	I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2; // can be ignored since we used I2C in standard mode


	I2C_Init(&I2C1Handle);
}

void GPIO_ButtonInit(void)
{
	GPIO_Handle_t GPIOBtn;

	GPIOBtn.pGPIOx = GPIOA;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0; // user button connected to PA0
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	//Initialization of the GPIO port
	GPIO_Init(&GPIOBtn);

}


int main (void)
{
	uint8_t commandCode, data_len;

	//Button init
	GPIO_ButtonInit();

	//I2C Pins inits
	I2C1_GPIOInits();

	//I2C peripheral configuration
	I2C1_Inits();

	//enables  the I2C peripheral (set PE)
	I2C_PeripheralControl(I2C1, ENABLE);

	//ACK bit is set only after PE is set, otherwise it is clear by the hardware
	I2C_ManageAcking(I2C1, I2C_ACK_ENABLE);

	while(1)
	{
		//wait till button is pressed
		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0) );

		//to avoid button de-bouncing related issues: 200mms of delay
		delay();

		commandCode = 0x51;

		//command 0x51 sent from master to slave to read the length of the data
		I2C_MasterSendData(&I2C1Handle, &commandCode, 1, SLAVE_ADDR, I2C_ENABLE_SR);

		//reception of the length of data to be send
		I2C_MasterReceiveData(&I2C1Handle, &data_len, 1, SLAVE_ADDR, I2C_ENABLE_SR);

		commandCode = 0x52;

		//command 0x52 sent from master to slave to read the data
		I2C_MasterSendData(&I2C1Handle, &commandCode, 1, SLAVE_ADDR, I2C_ENABLE_SR);

		//reception of the whole data
		I2C_MasterReceiveData(&I2C1Handle, rcv_buffer, data_len, SLAVE_ADDR, I2C_DISABLE_SR);

		rcv_buffer[data_len +1] = '\0';
	}
}
