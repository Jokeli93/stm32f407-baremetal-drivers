/*
 * 002LED_button.c
 *
 *  Created on: 22.02.2026
 *      Author: Joelikane
 */


#include "stm32f407xx.h"
#include "stm32f407xx_gpio_driver.h"

#define HIGH	1
#define LOW		0
#define BTN_PRESSED		LOW

void delay(void)
{
	for(uint32_t i = 0; i < 500000 / 2; i++);
}


int main (void)
{

	GPIO_Handle_t GpioLed, GPIOBtn;

	//GPIO port configuration of the LED
	GpioLed.pGPIOx = GPIOA;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8; //LED connected to PA14
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
	GpioLed.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_PP; //LED toggling with PUSH PULL configuration
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GpioLed.pGPIOx, ENABLE);

	//initialization of the LED
	GPIO_Init(&GpioLed);

	//GPIO port configuration of the external button
	GPIOBtn.pGPIOx = GPIOB;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12; // user button connected to PB12
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	//Enable the clock
	GPIO_PeriClockControl(GPIOBtn.pGPIOx, ENABLE);

	//Initialization of the button
	GPIO_Init(&GPIOBtn);

	while(1)
	{
		if(GPIO_ReadFromInputPin(GPIOBtn.pGPIOx, GPIOBtn.GPIO_PinConfig.GPIO_PinNumber) == BTN_PRESSED)
		{
			delay();
			//Toggle the GPIO pin
			GPIO_ToggleOutputPin(GpioLed.pGPIOx, GpioLed.GPIO_PinConfig.GPIO_PinNumber);

		}

	}

	return 0;
}
