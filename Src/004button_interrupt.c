/*
 * 002LED_button.c
 *
 *  Created on: 22.02.2026
 *      Author: Joelikane
 */

#include<string.h>
#include "stm32f407xx.h"
#include "stm32f407xx_gpio_driver.h"

#define HIGH	1
#define LOW		0
#define BTN_PRESSED		LOW

void delay(void)
{
	//this will introduce ~200ms delay when system clock is 16MHz
	for(uint32_t i = 0; i < 500000 / 2; i++);
}

GPIO_Handle_t GpioLed, GPIOBtn;

int main (void)
{

	//GPIO_Handle_t GpioLed, GPIOBtn;
	//memset(&GpioLed, 0, sizeof(GpioLed)); //Initialize the elements of the structure to 0 to avoid memory corruption of the register
	//memset(&GPIOBtn, 0, sizeof(GPIOBtn));

	//GPIO port configuration of the LED on pin PD12
	GpioLed.pGPIOx = GPIOD;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
	GpioLed.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_PP; //LED toggling with PUSH PULL configuration
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GpioLed.pGPIOx, ENABLE);

	//initialization of the LED
	GPIO_Init(&GpioLed);

	//GPIO port configuration of the external button on PD5
	GPIOBtn.pGPIOx = GPIOD;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT; //configuration mode as interrupt falling edge
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	//Enable the clock
	GPIO_PeriClockControl(GPIOBtn.pGPIOx, ENABLE);

	//Initialization of the button
	GPIO_Init(&GPIOBtn);

	//IRQ configurations
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI9_5, NVIC_IRQ_PRI15);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI9_5, ENABLE);

	while(1);

	return 0;
}


void EXTI9_5_IRQHandler(void)
{
	//delay();
	GPIO_IRQHandling(GPIO_PIN_NO_5); //clear the pending event from exti line
	GPIO_ToggleOutputPin(GpioLed.pGPIOx, GpioLed.GPIO_PinConfig.GPIO_PinNumber);
}
