/*
 * stm32f407xx_usart_driver.c
 *
 *  Created on: 05.04.2026
 *      Author: Joelikane
 */

#include "stm32f407xx_usart_driver.h"

//Peripheral Clock setup

/*******************************************************************************
* @fn			- USART_PeriClockControl
*
* @brief		- This function enables or disables peripheral clock for USART
*
*  @param[in]	- base address of the USART peripheral
*  @param[in]	- ENABLE or DISABLE macros
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
void USART_PeriClockControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pUSARTx == USART1)
		{
			USART1_PCLK_EN();
		}
		else if(pUSARTx == USART2)
		{
			USART2_PCLK_EN();
		}
		else if(pUSARTx == USART3)
		{
			USART3_PCLK_EN();
		}
		else if(pUSARTx == UART4)
		{
			UART4_PCLK_EN();
		}
		else if(pUSARTx == UART5)
		{
			UART5_PCLK_EN();
		}
		else if(pUSARTx == USART6)
		{
			USART6_PCLK_EN();
		}
	}
	else
	{
		if(pUSARTx == USART1)
		{
			USART1_PCLK_DI();
		}
		else if(pUSARTx == USART2)
		{
			USART2_PCLK_DI();
		}
		else if(pUSARTx == USART3)
		{
			USART3_PCLK_DI();
		}
		else if(pUSARTx == UART4)
		{
			UART4_PCLK_DI();
		}
		else if(pUSARTx == UART5)
		{
			UART5_PCLK_DI();
		}
		else if(pUSARTx == USART6)
		{
			USART6_PCLK_DI();
		}
	}
}

/*********************************************************************
 * @fn      		  - USART_Init
 *
 * @brief             - This function initializes/sets the given USART peripheral
 *
 * @param[in]         - Handle structure of the given USART peripheral
 *
 * @return            -
 *
 * @Note              -

 */
void USART_Init(USART_Handle_t *pUSARTHandle)
{

	//Temporary variable
	uint32_t temReg = 0;

	//enable the Clock for given USART peripheral
	USART_PeriClockControl(pUSARTHandle->pUSARTx, ENABLE);


/******************************** Configuration of CR1******************************************/

	//Enable USART Tx and Rx engines according to the USART_Mode configuration item
	if (pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_RX)
	{
		//enable the Receiver bit field
		temReg |= (1 << USART_CR1_RE);

	}else if (pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_TX)
	{
		//enable the Transmitter bit field
		temReg |= (1 << USART_CR1_TE);

	}else if (pUSARTHandle->USART_Config.USART_Mode == USART_MODE_TXRX)
	{
		//Implement the code to enable the both Transmitter and Receiver bit fields
		temReg |= ((1 << USART_CR1_RE) | (1 << USART_CR1_TE));
	}

    //configure the Word length item
	if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
	{
		//9 data bit configuration
		temReg |= (pUSARTHandle->USART_Config.USART_WordLength << USART_CR1_M) ;
	}

    //Configuration of parity control bit fields
	if (pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_EVEN)
	{
		//enable the parity control
		temReg |= (1 << USART_CR1_PCE);

		//Implement the code to enable EVEN parity
		//enable EVEN parity not required because by default EVEN parity will be selected once parity control is enabled

	}else if (pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_ODD)
	{
		//enable the parity control
	    temReg |= (1 << USART_CR1_PCE);

	    //Implement the code to enable ODD parity
	    temReg |= (1 << USART_CR1_PS);

	}

   //Program the CR1 register
	pUSARTHandle->pUSARTx->CR1 = temReg;

/******************************** Configuration of CR2******************************************/

	temReg=0;

	//configure the number of stop bits inserted during USART frame transmission
	temReg |= pUSARTHandle->USART_Config.USART_NoOfStopBits << USART_CR2_STOP;

	//Program the CR2 register
	pUSARTHandle->pUSARTx->CR2 = temReg;

/******************************** Configuration of CR3******************************************/

	temReg=0;

	//Configuration of USART hardware flow control
	if (pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS)
	{
		//enable CTS flow control
		temReg |= (1 << USART_CR3_CTSE);


	}else if (pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_RTS)
	{
		//enable RTS flow control
		temReg |= (1 << USART_CR3_RTSE);

	}else if (pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS_RTS)
	{
		//enable both CTS and RTS Flow control
		temReg |= ((1 << USART_CR3_CTSE) | (1 << USART_CR3_RTSE));
	}

	//Program the CR3 register
	pUSARTHandle->pUSARTx->CR3 = temReg;

/******************************** Configuration of BRR(Baud rate register)******************************************/

	//Implement the code to configure the baud rate
	//We will cover this in the lecture. No action required here

}

/*******************************************************************************
* @fn			- USART_DeInit
*
* @brief		- This function resets the given USART peripheral
*
*  @param[in]	- base address of the USART peripheral
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
void USART_DeInit(USART_RegDef_t *pUSARTx)
{
	if(pUSARTx == USART1)
	{
		USART1_REG_RESET();
	}
	else if(pUSARTx == USART2)
	{
		USART2_REG_RESET();
	}
	else if(pUSARTx == USART3)
	{
		USART3_REG_RESET();
	}
	else if(pUSARTx == UART4)
	{
		UART4_REG_RESET();
	}
	else if(pUSARTx == UART5)
	{
		UART5_REG_RESET();
	}
	else if(pUSARTx == USART6)
	{
		USART6_REG_RESET();
	}
}

/*********************************************************************
 * @fn      		  - USART_SendData
 *
 * @brief             - This function is used to send the data
 *
 * @param[in]         - USART handle structure
 * @param[in]         - transmission buffer
 * @param[in]         - data length to transmit
 *
 * @return            -
 *
 * @Note              -

 */
void USART_SendData(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t len)
{

	uint16_t *pdata;
   //Loop over until "len" number of bytes are transferred
	for(uint32_t i = 0 ; i < len; i++)
	{
		//wait until TXE flag is set in the SR
		while(! USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_TXE));

         //Check the USART_WordLength item for 9BIT or 8BIT in a frame
		if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
		{
			//if 9BIT, load the DR with 2bytes masking the bits other than first 9 bits
			pdata = (uint16_t*)pTxBuffer;
			pUSARTHandle->pUSARTx->DR = (*pdata & (uint16_t)0x01FF);

			//check for USART_ParityControl
			if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
			{
				//No parity is used in this transfer. so, 9bits of user data will be sent
				//increment pTxBuffer twice
				pTxBuffer++;
				pTxBuffer++;
			}
			else
			{
				//Parity bit is used in this transfer . so , 8bits of user data will be sent
				//The 9th bit will be replaced by parity bit by the hardware
				pTxBuffer++;
			}
		}
		else
		{
			//This is 8bit data transfer
			pUSARTHandle->pUSARTx->DR = (*pTxBuffer  & (uint8_t)0xFF);

			//increment the buffer address
			pTxBuffer++;
		}
	}

	//wait till TC flag is set in the SR
	while( ! USART_GetFlagStatus(pUSARTHandle->pUSARTx,USART_FLAG_TC));
}

//IRQ configuration and ISR handling

/*******************************************************************************
* @fn			- USART_IRQInterruptConfig
*
* @brief		-This function enables/disables the interrupt of a given IRQ number
*
*  @param[in]	- IRQ number
*  @param[in]	- ENABLE or DISABLE macro
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
void USART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if (EnorDi == ENABLE)
	{
		if (IRQNumber <= 31)
		{
			//Programs ISER0
			*NVIC_ISER0 |= (1 << IRQNumber);

		}
		else if(IRQNumber > 31 && IRQNumber <= 63)
		{
			//Programs ISER1
			*NVIC_ISER1 |= (1 << IRQNumber % 32);

		}
		else if(IRQNumber > 63 && IRQNumber <= 95)
		{
			//Programs ISER2
			*NVIC_ISER2 |= (1 << IRQNumber % 64);

		}
	}
	else
	{
		if (IRQNumber <= 31)
		{
			//Programs ICER0
			*NVIC_ICER0 |= (1 << IRQNumber);

		}
		else if(IRQNumber > 31 && IRQNumber <= 63)
		{
			//Programs ICER1
			*NVIC_ICER1 |= (1 << IRQNumber % 32);

		}
		else if(IRQNumber > 63 && IRQNumber <= 95)
		{
			//Programs ICER2
			*NVIC_ICER2 |= (1 << IRQNumber % 64);

		}
	}

}

/*******************************************************************************
* @fn			- USART_IRQPriorityConfig
*
* @brief		-This function configures the priority of a given interrupt
*
*  @param[in]	- IQR number
*  @param[in]	- IQR priority
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
void USART_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_section) + (8- NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}

/*******************************************************************************
* @fn		USART_PeripheralControl
*
* @brief		- This function enables or disables the USART peripheral
*
*  @param[in]	- base address of the USART peripheral
*  @param[in]	- ENABLE or DISABLE macros
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
void USART_PeripheralControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
		pUSARTx->CR1 |= (1 << USART_CR1_UE);
	else
		pUSARTx->CR1 &= ~(1 << USART_CR1_UE);
}

/*******************************************************************************
* @fn		USART_GetFlagStatus
*
* @brief		- This function gets the status of any given flag in the SR register
*
*  @param[in]	- base address of the USART peripheral
*  @param[in]	- Flag name
*
* @return		- flag status
*
* @note			- none
*
********************************************************************************/
uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx , uint32_t flagName)
{
	if(pUSARTx->SR & flagName)
	{
		return FLAG_SET;
	}

	return FLAG_RESET;
}

/*******************************************************************************
* @fn		USART_ClearFlag
*
* @brief		- This function clears any given flag in the SR register
*
*  @param[in]	- base address of the USART peripheral
*  @param[in]	- Flag name
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
void USART_ClearFlag(USART_RegDef_t *pUSARTx, uint32_t flagName)
{
	if(pUSARTx->SR &  flagName)
	{
		pUSARTx->SR &= ~flagName;
	}
}

