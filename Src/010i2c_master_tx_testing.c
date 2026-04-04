/*
 * 010i2c_master_tx_testing.c
 *
 *  Created on: 18.03.2026
 *      Author: Joelikane
 */


/*
 *
 * I2C Master (STM32F407xx) and I2C Slave (Arduino) communication
 *
 * When button on the STM32 board (master) is pressed, master should send data on the
 * Arduion board (slave). The data received by the Arduino board will be displayed on
 * the serial monitor of the Arduino IDE.
 *
 * 1. Use I2C_SCL = 100kHz (Standart mode)
 * 2. Use external pull up resistors (3.3 KOhm) for SDA and SCL line
 *
 * Note: If external pull up resistors are not available, simply activate the STM32 I2C
 *       pin's internal pull up resistors.
 */


#include<stdio.h>
#include<string.h>
#include "stm32f407xx.h"


void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i ++);
}

I2C_Handle_t I2C1Handle;

#define MY_ADDR			0x61

#define SLAVE_ADDR		0x68

/*
 * Some  data to send
 *
 * Note: The Arduino sketch is written using Arduino wWire library. The Wire library has limitation on how
 *       many bytes can be transferd or received in single I2C transaction and the limit is 32 bytes.
 *       So, don't send/receive more than 32 bytes in single I2C transaction.
 */
uint8_t sample_data[]= "We are testing I2C master Tx\n";


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

	//Button init
	GPIO_ButtonInit();

	//I2C Pins inits
	I2C1_GPIOInits();

	//I2C peripheral configuration
	I2C1_Inits();

	//enables  the I2C peripheral
	I2C_PeripheralControl(I2C1, ENABLE);

	while(1)
	{
		//wait till button is pressed
		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0) );

		//to avoid button de-bouncing related issues: 200mms of delay
		delay();

		//send the data
		I2C_MasterSendData(&I2C1Handle, sample_data, strlen((char*)sample_data), SLAVE_ADDR);

	}
}
