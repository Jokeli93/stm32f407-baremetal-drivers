/*
 * 008spi_cmd_handling.c
 *
 *SPi Master (STM) amd SPI Slave (Arduino) command & response based communication.
 *
 *When the user button on the master is pressed, master send a command to the slave and slave responds
 *as per the command implementation.
 *1. Use SPI Full duplex
 *2. STM board in master mode and Arduino in slave mode
 *3. DFF = 0 (1 byte data  frame)
 *4. Use hardware slave management (SSM = 0)
 *5. SCLK speed = 2MHz, fclk = 16MHz
 *4.
 *Power the arduino board and download SPI slave sketch
 *sketch folder: test/Arduino/SPI/002SPISlaveCmdHandling/002SPISlaveCmdHandling.ino
 *
 *  Created on: 09.03.2026
 *      Author: Joelikane
 */

#include <stdio.h>
#include <string.h>
#include "stm32f407xx.h"

//extern void initialise_monitor_handles();

//Command codes
#define COMMAND_LED_CTRL			0x50
#define COMMAND_SENSOR_READ			0x51
#define COMMAND_LED_READ			0x52
#define COMMAND_PRINT				0x53
#define COMMAND_ID_READ				0x54

#define LED_ON		1
#define LED_OFF		0

//Arduino analog pins
#define ANALOG_PIN0		0
#define ANALOG_PIN1		1
#define ANALOG_PIN2		2
#define ANALOG_PIN3		3
#define ANALOG_PIN4		4
#define ANALOG_PIN5		5

//arduino LED
#define LED_PIN		9

/*
 * PB14 --> SPI2_MISO
 * PB15 --> SPI2_MOSI
 * PB13 --> SPI2_SCLK
 * PB12 --> SPI2_NSS
 * ALT function mode: 5
 */
void SPI2_GPIOInits(void)
{
	GPIO_Handle_t SPIPins;

	SPIPins.pGPIOx = GPIOB;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinaltFunMode = 5;
	SPIPins.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;

	//SCLK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPIPins);

	//MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPIPins);

	//MISO
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	GPIO_Init(&SPIPins);

	//NSS
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&SPIPins);
}

void SPI2_Inits(void)
{
	SPI_Handle_t SPI2Handle;

	SPI2Handle.pSPIx = SPI2;
	SPI2Handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2Handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2Handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV8; //generate sclk of 2MHz
	SPI2Handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2Handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2Handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW; //first clock transition is the first data capture
	SPI2Handle.SPIConfig.SPI_SSM = SPI_SSM_DI; //hardware slave management enabled for NSS

	SPI_Init(&SPI2Handle);
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

uint8_t SPI_VerifyResponse(uint8_t ackbyte)
{

	if(ackbyte == (uint8_t)0x5F)
		//ack
		return 1;
	else
		return 0;
}

void delay(void)
{
	for(uint32_t i = 0; i < 500000 / 2; i++);
}

int main(void)
{

	//initialise_monitor_handles();

	printf("Application is running\n");

	//This function is used to initialize the GPIO button (USER button)
	GPIO_ButtonInit();

	//This function is used to initialize the GPIO pins to behaves as SPI2 pins
	SPI2_GPIOInits();

	//This function is used to initialize the SPI2 peripheral parameters
	SPI2_Inits();

	printf("SPI Init. done\n");

	//this makes NSS signal internally high and avoids MODF error
	//SPI_SSIConfig(SPI2, ENABLE);

	/*
	 * making SSOE 1 does NSS output enable.
	 * The NSS pin  is automatically managed by the hardware.
	 * i.e when SPE = 1, NSS will be pulled to low
	 * and NSS pin will be high when SPE = 0
	 */
	SPI_SSOEConfig(SPI2, ENABLE);


	while(1)
	{

		//dummy byte
		uint8_t dummy_write = 0xff;
		uint8_t dummy_read;

		//wait till button is pressed
		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0) );

		//to avoid button de-bouncing related issues: 200mms of delay
		delay();

		//enables the  SPI2 peripheral
		SPI_PeripheralControl(SPI2, ENABLE);


		//1. CMD_LED_CTRL  <pin no(1)>  <value(1)>

		uint8_t commandCode = COMMAND_LED_CTRL;
		uint8_t ackByte;
		uint8_t args[2];

		//send the command
		SPI_SendData(SPI2, &commandCode, 1);

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI2, &dummy_read, 1);

		//Send some dummy bits (1 byte) to fetch the response from the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		//Read the ack byte received
		SPI_ReceiveData(SPI2, &ackByte, 1);

		if(SPI_VerifyResponse(ackByte))
		{
			//send arguments
			args[0] = LED_PIN;
			args[1] = LED_ON;
			SPI_SendData(SPI2, args, 2);

			// dummy read to clear RXNE flag
			SPI_ReceiveData(SPI2,args,2);

			//printf("COMMAND_LED_CTRL Executed\n");

		}
		//end of COMMAND_LED_CTRL


		//2. COMMAND_SENSOR_READ

		//wait till button is pressed
		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0) );

		//to avoid button de-bouncing related issues: 200mms of delay
		delay();

		commandCode = COMMAND_SENSOR_READ;

		//send the command
		SPI_SendData(SPI2, &commandCode, 1);

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI2, &dummy_read, 1);

		//Send some dummy bits (1 byte) to fetch the response from the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		//Read the ack byte received
		SPI_ReceiveData(SPI2, &ackByte, 1);

		if(SPI_VerifyResponse(ackByte))
		{
			//send arguments
			args[0] = ANALOG_PIN0;
			SPI_SendData(SPI2, args, 1); //send 1 byte of data

			//do dummy read to clear off the RXNE
			SPI_ReceiveData(SPI2, &dummy_read, 1);

			//insert some dalay so that slave can be ready with the data
			delay();

			//Send some dummy bits (1 byte) to fetch the sensor analog value
			SPI_SendData(SPI2, &dummy_write, 1);

			uint8_t analog_read;
			SPI_ReceiveData(SPI2, &analog_read, 1);

			//printf("COMMAND_SENSOR_READ %d\n",analog_read);

		}


		//3. COMMAND_LED_READ <pin no(1)>
		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0) );

		//to avoid button de-bouncing related issues: 200mms of delay
		delay();

		commandCode = COMMAND_LED_READ;

		//send command
		SPI_SendData(SPI2, &commandCode, 1);

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI2, &dummy_read, 1);

		//Send some dummy bits (1 byte) to fetch the response from the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		//Read the ack byte received
		SPI_ReceiveData(SPI2, &ackByte, 1);

		if(SPI_VerifyResponse(ackByte))
		{
			//send arguments
			args[0] = LED_PIN;
			SPI_SendData(SPI2, args, 1); //send 1 byte of data

			//do dummy read to clear off the RXNE
			SPI_ReceiveData(SPI2, &dummy_read, 1);

			//insert some dalay so that slave can be ready with the data
			delay();

			//Send some dummy bits (1 byte) to fetch the response from the slave
			SPI_SendData(SPI2, &dummy_write, 1);

			uint8_t led_sattus;
			SPI_ReceiveData(SPI2, &led_sattus, 1);

			//printf("COMMAND_READ_LED %d\n",led_sattus);

		}


		//4. COMMAND_PRINT   <len(2)>  <message(1)>
		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0) );

		//to avoid button de-bouncing related issues: 200mms of delay
		delay();

		commandCode = COMMAND_PRINT;

		//send command
		SPI_SendData(SPI2, &commandCode, 1);

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI2, &dummy_read, 1);

		//Send some dummy bits (1 byte) to fetch the response from the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		//Read the ack byte received
		SPI_ReceiveData(SPI2, &ackByte, 1);

		uint8_t message [] = "Hello ! How are you ??";

		if(SPI_VerifyResponse(ackByte))
		{
			//send arguments
			args[0] = strlen((char*)message);
			SPI_SendData(SPI2, args, 1); //send 1 byte of data

			//do dummy read to clear off the RXNE
			SPI_ReceiveData(SPI2, &dummy_read, 1);

			//insert some dalay so that slave can be ready with the data
			delay();

			//send message

			for(int i = 0; i < args[0]; i++)
			{
				SPI_SendData(SPI2, &message[i], 1);
				SPI_ReceiveData(SPI2, &dummy_read, 1);
			}

			//printf("COMMAND_PRINT executed \n");

		}


		//5. COMMAND_ID_READ
		//wait till button is pressed
		while( ! GPIO_ReadFromInputPin(GPIOA,GPIO_PIN_NO_0) );

		//to avoid button de-bouncing related issues 200ms of delay
		delay();

		commandCode = COMMAND_ID_READ;

		//send command
		SPI_SendData(SPI2, &commandCode, 1);

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI2, &dummy_read, 1);

		//Send some dummy byte to fetch the response from the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		//read the ack byte received
		SPI_ReceiveData(SPI2, &ackByte, 1);

		uint8_t id[11];
		uint32_t i = 0;

		if(SPI_VerifyResponse(ackByte))
		{
			//read 10 bytes id from the slave
			for(i = 0; i < 10; i++)
			{
				SPI_SendData(SPI2, &dummy_write, 1);
				SPI_ReceiveData(SPI2, &id[i], 1);
			}

			id[10] = '\0';

			//printf("COMMAND_ID : %s \n", id);
		}


		//lets confirm SPI is not busy
		while(SPI_GetFlagStatus(SPI2, SPI_BUSY_FLAG));

		//disables the  SPI2 peripheral
		SPI_PeripheralControl(SPI2, DISABLE);

		//printf("SPI communication closed\n");
	}

	return 0;
}
