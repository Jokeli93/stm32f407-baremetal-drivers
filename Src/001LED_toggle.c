/*
 * 001LED_toggle.c
 *
 *  Created on: 22.02.2026
 *      Author: Joelikane
 */
#include "stm32f407xx.h"
#include "stm32f407xx_gpio_driver.h"

void delay(void)
{
	for(uint32_t i = 0; i < 500000; i++);
}


int main (void)
{
	//Definition of the handle structure
	GPIO_Handle_t GpioLed;

	GpioLed.pGPIOx = GPIOD;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
	GpioLed.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_PP; //LED toggling with PUSH PULL configuration
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	//Enable the clock
	GPIO_PeriClockControl(GpioLed.pGPIOx, ENABLE);

	//Initialization of the GPIO port
	GPIO_Init(&GpioLed);

	while(1)
	{
		//Toggle the GPIO pin
		GPIO_ToggleOutputPin(GpioLed.pGPIOx, GpioLed.GPIO_PinConfig.GPIO_PinNumber);

		delay();
	}

	return 0;
}
