/*
 * 012i2c_master_rx_testingIT.c
 *
 *  Created on: 27.03.2026
 *      Author: Joelikane
 */


/*
 * I2C Master (STM32F407xx) and I2C Slave (Arduino) communication in Interrupt mode.
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

//Flag variable
uint8_t rxComplt = RESET;

#define MY_ADDR			0x61

#define SLAVE_ADDR		0x68 // Used SLAVE_ADDR = 0x61 as example to observe the ACK failure

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
	I2C1Handle.I2C_Config.I2C_FMDutyDycle = I2C_FM_DUTY_2; // can be ignored since we used I2C in standard mode


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

	//I2C IRQ configurations
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);

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
		while(I2C_MasterSendDataIT(&I2C1Handle, &commandCode, 1, SLAVE_ADDR, I2C_ENABLE_SR) != I2C_READY);

		//reception of the length of data to be send
		while(I2C_MasterReceiveDataIT(&I2C1Handle, &data_len, 1, SLAVE_ADDR, I2C_ENABLE_SR) != I2C_READY);


		commandCode = 0x52;

		//command 0x52 sent from master to slave to read the data
		while(I2C_MasterSendDataIT(&I2C1Handle, &commandCode, 1, SLAVE_ADDR, I2C_ENABLE_SR) != I2C_READY);

		//reception of the whole data
		while(I2C_MasterReceiveDataIT(&I2C1Handle, rcv_buffer, data_len, SLAVE_ADDR, I2C_DISABLE_SR) != I2C_READY);

		rxComplt = RESET;

		//wait until reception completes
		while(rxComplt != SET);

		rcv_buffer[data_len +1] = '\0';

		printf("Data: %s", rcv_buffer);

		rxComplt = RESET;
	}
}

void I2C1_EV_IRQHandler(void)
{
	I2C_EV_IRQHandling(&I2C1Handle);
}

void I2C1_ER_IRQHandler(void)
{
	I2C_ER_IRQHandling(&I2C1Handle);
}

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEvt)
{
	if(AppEvt == I2C_EV_TX_CMPLT)
	{
		printf("Tx is complete\n");
	}
	else if(AppEvt == I2C_EV_RX_CMPLT)
	{
		printf("Rx is completed\n");

		rxComplt = SET;
	}
	if(AppEvt == I2C_ERROR_AF)
	{
		printf("Error: Ack failure\n");
		//in master, ACK failure happens when slave fails to send ACK for the byte sent from master.
		I2C_CloseSendData(pI2CHandle);

		//generate the stop condition
		I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

		//Hang in infinite loop
		while(1);
	}
}
