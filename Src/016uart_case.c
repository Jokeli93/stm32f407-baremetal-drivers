/*
 * 016uart_case.c
 *
 *  Created on: 07.04.2026
 *      Author: Joelikane
 */


/*
 * UART communication between STM32 board and  Arduino board
 *
 * This program is used for STM32 board to transmit different messages to the Arduino board
 *
 * For every message STM32 board  sends, Arduino code will change the case of
 * alphabets (lower to upper case and vice versa) and sends message back to the STM32 board.
 *
 * The STM32 board should capture the reply to the Arduino and  display using  semi hosting.
 *
 * For the Arduino board use the sketch "test\Arduino\USART\002UARTTxString\002UARTTxString.ino"
 * located in this repository
 */


#include<stdio.h>
#include<string.h>
#include "stm32f407xx.h"



//we have 3 different messages that we transmit to arduino
//add more messages as needed
char *msg[3] = {"hihihihihihi123", "Hello How are you ?" , "Today is Monday !"};

//reply from arduino will be stored here
char rx_buf[1024] ;

USART_Handle_t usart2_handle;


//This flag indicates reception completion
uint8_t rxCmplt = RESET;

uint8_t g_data = 0;

extern void initialise_monitor_handles();

void USART2_Init(void)
{
	usart2_handle.pUSARTx = USART2;
	usart2_handle.USART_Config.USART_Baud = USART_STD_BAUD_115200;
	usart2_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
	usart2_handle.USART_Config.USART_Mode = USART_MODE_TXRX;
	usart2_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	usart2_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
	usart2_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;

	USART_Init(&usart2_handle);
}

void USART2_GPIOInit(void)
{
	GPIO_Handle_t usart_gpios;

	usart_gpios.pGPIOx = GPIOA;
	usart_gpios.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	usart_gpios.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_PP;
	usart_gpios.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	usart_gpios.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
	usart_gpios.GPIO_PinConfig.GPIO_PinaltFunMode =7;

	//USART2 TX
	usart_gpios.GPIO_PinConfig.GPIO_PinNumber  = GPIO_PIN_NO_2;
	GPIO_Init(&usart_gpios);

	//USART2 RX
	usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
	GPIO_Init(&usart_gpios);

}

void GPIO_ButtonInit(void)
{
	GPIO_Handle_t GPIOBtn,GpioLed;

	//this is btn gpio configuration
	GPIOBtn.pGPIOx = GPIOA;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&GPIOBtn);

	//this is led gpio configuration
	GpioLed.pGPIOx = GPIOD;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
	GpioLed.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_OD;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOD,ENABLE);

	GPIO_Init(&GpioLed);

}

void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i ++);
}


int main(void)
{
	uint32_t cnt = 0;


	initialise_monitor_handles();

	USART2_GPIOInit();

    USART2_Init();

    USART_IRQInterruptConfig(IRQ_NO_USART2, ENABLE);

    USART_PeripheralControl(USART2, ENABLE);

    printf("Application is running\n");

    //do forever
    while(1)
    {
		//wait till button is pressed
		while(! GPIO_ReadFromInputPin(GPIOA,GPIO_PIN_NO_0));

		//to avoid button de-bouncing related issues 200ms of delay
		delay();

		// Next message index ; make sure that cnt value doesn't cross 2
		cnt = cnt % 3;

		//First lets enable the reception in interrupt mode
		//this code enables the receive interrupt
		while (USART_ReceiveDataIT(&usart2_handle, rx_buf, strlen(msg[cnt])) != USART_READY);

		//Send the msg indexed by cnt in blocking mode
    	USART_SendData(&usart2_handle, (uint8_t*)msg[cnt], strlen(msg[cnt]));

    	printf("Transmitted : %s\n", msg[cnt]);


    	//Now lets wait until all the bytes are received from the arduino .
    	//When all the bytes are received rxCmplt will be SET in application callback
    	while(rxCmplt != SET);

    	//just make sure that last byte should be null otherwise %s fails while printing
    	rx_buf[strlen(msg[cnt])+ 1] = '\0';

    	//Print what we received from the arduino
    	printf("Received    : %s\n", rx_buf);

    	//invalidate the flag
    	rxCmplt = RESET;

    	//move on to next message indexed in msg[]
    	cnt ++;
    }

	return 0;
}


void USART2_IRQHandler(void)
{
	USART_IRQHandling(&usart2_handle);
}





void USART_ApplicationEventCallback( USART_Handle_t *pUSARTHandle,uint8_t ApEv)
{
   if(ApEv == USART_EVENT_RX_CMPLT)
   {
			rxCmplt = SET;

   }else if (ApEv == USART_EVENT_TX_CMPLT)
   {
	   ;
   }
}
