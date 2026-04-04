/*
 * 013i2c_slave_tx_string.c
 *
 *  Created on: 04.04.2026
 *      Author: Joelikane
 */


/*
 * I2C Master (Arduino) and I2C Slave (STM32F407xx) communication in Interrupt mode.
 *
 *Master should read and display data from STM32 slave connected. First master has to get
 *the length of the data from the slave to read subsequent data from the slave.
 *
 *1. Use I2C SCL = 100KHz(Standard mode)
 *2. Use internal pull up resistors for SDA and SCL lines

 *Procedure to read the data from the slave
 *
 *1. Master(Arduino) sends command code 0x51 to read the length (1 byte) of the data from the slave(STM32)
 *2. Master send command 0x52 to read the complete data from the slave
*/



#include<stdio.h>
#include<string.h>
#include "stm32f407xx.h"


#define SLAVE_ADDR		0x68 // Used SLAVE_ADDR = 0x63 as example to observe the ACK failure

#define MY_ADDR			SLAVE_ADDR

void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i ++);
}

I2C_Handle_t I2C1Handle;

//Tx buffer
uint8_t Tx_buffer[32] = "STM32 Slave mode testing...";


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

	//I2C IRQ configurations
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);

	I2C_SlaveEnableDisableCallbackEvents(I2C1, ENABLE);

	//enables  the I2C peripheral (set PE)
	I2C_PeripheralControl(I2C1, ENABLE);

	//ACK bit is set only after PE is set, otherwise it is cleared by the hardware
	I2C_ManageAcking(I2C1, I2C_ACK_ENABLE);

	while(1);

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
	static uint8_t commandCode = 0;
	static uint8_t cnt = 0;

	if(AppEvt == I2C_EV_DATA_REQ)
	{
		//Master wants some data from the slave
		if(commandCode == 0x51)
		{
			//send the length information to the master
			I2C_SlaveSendData(I2C1, strlen((char*)Tx_buffer));
		}
		else if(commandCode == 0x52)
		{
			//send the content of the data (@Tx_buffer)
			I2C_SlaveSendData(I2C1, Tx_buffer[cnt++]);
		}
	}
	else if(AppEvt == I2C_EV_DATA_RCV)
	{
		//Data is waiting for the slave to read
		commandCode = I2C_SlaveReceiveData(I2C1);
	}
	else if(AppEvt == I2C_ERROR_AF)
	{
		//this happens only during slave transmission to master
		//Master sends the NACK and slave understands that the master doesn't need more data.
		commandCode = 0xff;
		cnt = 0;
	}
	else if(AppEvt == I2C_EV_STOP)
	{
		//this happens only during slave reception
		//Master ends the I2C communication with the slave
	}
}
