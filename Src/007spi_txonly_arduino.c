/*
 * 007spi_txonly_arduino.c
 *
 *  Created on: 04.03.2026
 *      Author: Joelikane
 */


#include <string.h>
#include "stm32f407xx.h"

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
	//SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	//GPIO_Init(&SPIPins);

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

void GPIO_ButtonInit()
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

void delay(void)
{
	for(uint32_t i = 0; i < 500000 / 2; i++);
}

int main(void)
{
	char user_data[] = "Hello World";

	//This function is used to initialize the GPIO button (USER button)
	GPIO_ButtonInit();

	//This function is used to initialize the GPIO pins to behaves as SPI2 pins
	SPI2_GPIOInits();

	//This function is used to initialize the SPI2 peripheral parameters
	SPI2_Inits();

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

		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0) );

		//to avoid button de-bouncing related issues: 200mms of delay
		delay();

		//enables the  SPI2 peripheral
		SPI_PeripheralControl(SPI2, ENABLE);

		//first send length information
		/*
		 * The Arduino sketch expects 1 byte of length
		 * information followed bythe data
		 */
		uint8_t dataLen = strlen(user_data);
		SPI_SendData(SPI2, &dataLen, 1); //send the information about the length of the data

		//send the user data
		SPI_SendData(SPI2, (uint8_t*)user_data, strlen(user_data));

		//lets confirm SPI is not busy
		while(SPI_GetFlagStatus(SPI2, SPI_BUSY_FLAG));

		//disables the  SPI2 peripheral
		SPI_PeripheralControl(SPI2, DISABLE);
	}

	return 0;
}

