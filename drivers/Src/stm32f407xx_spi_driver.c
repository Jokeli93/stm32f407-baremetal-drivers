/*
 * stm32f407xx_spi_driver.c
 *
 *  Created on: 28.02.2026
 *      Author: Joelikane
 */

#include "stm32f407xx_spi_driver.h"

static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle);

//Peripheral Clock setup

/*******************************************************************************
* @fn			- GPIO_PeriClockControl
*
* @brief		- This function enables or disables peripheral clock for SPI
*
*  @param[in]	- base address of the gpio peripheral
*  @param[in]	- ENABLE or DISABLE macros
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_EN();
		}
		else if(pSPIx == SPI2)
		{
			SPI2_PCLK_EN();
		}
		else if(pSPIx == SPI3)
		{
			SPI3_PCLK_EN();
		}
		else if(pSPIx == SPI4)
		{
			SPI4_PCLK_EN();
		}
		else if(pSPIx == SPI5)
		{
			SPI5_PCLK_EN();
		}
		else if(pSPIx == SPI6)
		{
			SPI6_PCLK_EN();
		}
	}
	else
	{
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_DI();
		}
		else if(pSPIx == SPI2)
		{
			SPI2_PCLK_DI();
		}
		else if(pSPIx == SPI3)
		{
			SPI3_PCLK_DI();
		}
		else if(pSPIx == SPI4)
		{
			SPI4_PCLK_DI();
		}
		else if(pSPIx == SPI5)
		{
			SPI5_PCLK_DI();
		}
		else if(pSPIx == SPI6)
		{
			SPI6_PCLK_DI();
		}
	}
}

//Init and De-Init

/*******************************************************************************
* @fn			- SPI_Init
*
* @brief		- This function initializes/sets the given SPI peripheral
*
*  @param[in]	- Structure to handle a GPIO pin
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
	//Configure the SPI_CR1 register

	uint32_t tempReg = 0;

	//enable the peripheral clock

	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);

	//1. Configure the device mode
	tempReg = pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR;

	//2. Configure the bus config
	if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		//clear the BIDI mode
		tempReg &= ~(1 << SPI_CR1_BIDIMODE);
	}
	else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		//set the BIDI mode
		tempReg |= (1 << SPI_CR1_BIDIMODE);
	}
	else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
	{
		//clear the BIDI mode
		tempReg &= ~(1 << SPI_CR1_BIDIMODE);

		//set the  RXONLY mode
		tempReg |= (1 << SPI_CR1_RXONLY);

	}

	//3. Configure the SPI serial clock speed (baud rate)
	tempReg |= pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR;

	//4. Configure the DFF
	tempReg |= pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF;

	//5. Configure the CPOL
	tempReg |= pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL;

	//6. Configure the CPHA
	tempReg |= pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA;

	//7.configure the SSM
	tempReg |= pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM;

	pSPIHandle->pSPIx->CR1 = tempReg;

}

/*******************************************************************************
* @fn			- SPI_DeInit
*
* @brief		- This function resets the given SPI peripheral
*
*  @param[in]	- base address of the gpio peripheral
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
	if(pSPIx == SPI1)
		{
			SPI1_REG_RESET();
		}
		else if(pSPIx == SPI2)
		{
			SPI2_REG_RESET();
		}
		else if(pSPIx == SPI3)
		{
			SPI3_REG_RESET();
		}
		else if(pSPIx == SPI4)
		{
			SPI4_REG_RESET();
		}
		else if(pSPIx == SPI5)
		{
			SPI5_REG_RESET();
		}
		else if(pSPIx == SPI6)
		{
			SPI6_REG_RESET();
		}
}

/*******************************************************************************
* @fn			- SPI_SendData
*
* @brief		- This function is used to send the data
*
*  @param[in]	- base address of the gpio peripheral
*  @param[in]	- transmission register
*  @param[in]	- lenght of the data
*
* @return		- none
*
* @note			- This is a blocking call (polling based code)
*
********************************************************************************/
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t len)
{
	while(len> 0)
	{
		//1. wait until TXE is set
		while(SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET); //Here we are polling for the TXE flag to SET

		//2. check the DFF bit in CR1
		if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
		{
			//16 bit DFF
			//1. load the data in to the DR
			pSPIx->DR = *((uint16_t*)pTxBuffer);
			len--;
			len--;
			(uint16_t*)pTxBuffer++;
		}
		else
		{
			//8 bit DFF
			pSPIx->DR = *pTxBuffer;
			len--;
			pTxBuffer++;
		}
	}
}

/*******************************************************************************
* @fn			- SPI_ReceiveData
*
* @brief		- This function is used to receive the data
*
*  @param[in]	- base address of the gpio peripheral
*  @param[in]	- receive register
*  @param[in]	- lenght of the data
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t len)
{
	while(len> 0)
	{
		//1. wait until RXNE is set
		while(SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET); //Here we are polling for the RXNE flag to SET

		//2. check the DFF bit in CR1
		if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
		{
			//16 bit DFF
			//1. Read  the data from the DR to Rx buffer address
			*((uint16_t*)pRxBuffer) = pSPIx->DR;
			len--;
			len--;
			(uint16_t*)pRxBuffer++;
		}
		else
		{
			//8 bit DFF
			*pRxBuffer = pSPIx->DR;
			len--;
			pRxBuffer++;
		}
	}
}

/*******************************************************************************
* @fn			- SPI_SendDataIT
*
* @brief		- This function is used to send the data in interrupt mode
*
*  @param[in]	- SPI handle structure
*  @param[in]	- transmit register
*  @param[in]	- length of the data
*
* @return		- Application state
*
* @note			- none
*
********************************************************************************/
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t len)
{
	uint8_t state = pSPIHandle->TxState;

	if(state != SPI_BUSY_IN_TX)
	{
		//1. Save the Tx buffer address and Len information in some global variables
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = len;

		//2. Mark the SPI state as busy in transmission so that no oder code can take over same
		//   SPI peripheral until transmission is over
		pSPIHandle->TxState = SPI_BUSY_IN_TX;

		//3. Enable TXEIE control bit to get interrupt whenever TXE flag is set in SR
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);

	}

	//4. Data transmission will be handle by the ISR code (to implement later)

	return state;

}

/*******************************************************************************
* @fn			- SPI_ReceiveDataIT
*
* @brief		- This function is used to receive the data in interrupt mode
*
*  @param[in]	- SPI handle structure
*  @param[in]	- receive register
*  @param[in]	- length of the data
*
* @return		- Application state
*
* @note			- none
*
********************************************************************************/
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t len)
{
	uint8_t state = pSPIHandle->RxState;

	if(state != SPI_BUSY_IN_RX)
	{
		//1. Save the Tx buffer address and Len information in some global variables
		pSPIHandle->pRxBuffer = pRxBuffer;
		pSPIHandle->RxLen = len;

		//2. Mark the SPI state as busy in reception so that no oder code can take over same
		//   SPI peripheral until reception is over
		pSPIHandle->RxState= SPI_BUSY_IN_RX;

		//3. Enable RXNEIE control bit to get interrupt whenever RXNE flag is set in SR
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);

	}

	//4. Data reception will be handle by the ISR code (to implement later)

	return state;

}

//IRQ configuration and ISR handling

/*******************************************************************************
* @fn			- SPI_IRQInterruptConfig
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
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if (EnorDi == ENABLE)
	{
		if (IRQNumber <= 31)
		{
			//Programm ISER0
			*NVIC_ISER0 |= (1 << IRQNumber);

		}
		else if(IRQNumber > 31 && IRQNumber <= 63)
		{
			//Programm ISER1
			*NVIC_ISER1 |= (1 << IRQNumber % 32);

		}
		else if(IRQNumber > 63 && IRQNumber <= 95)
		{
			//Programm ISER2
			*NVIC_ISER2 |= (1 << IRQNumber % 64);

		}
	}
	else
	{
		if (IRQNumber <= 31)
		{
			//Programm ICER0
			*NVIC_ICER0 |= (1 << IRQNumber);

		}
		else if(IRQNumber > 31 && IRQNumber <= 63)
		{
			//Programm ICER1
			*NVIC_ICER1 |= (1 << IRQNumber % 32);

		}
		else if(IRQNumber > 63 && IRQNumber <= 95)
		{
			//Programm ICER2
			*NVIC_ICER2 |= (1 << IRQNumber % 64);

		}
	}

}

/*******************************************************************************
* @fn			- SPI_IRQPriorityConfig
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
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_section) + (8- NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}

/*******************************************************************************
* @fn		SPI_IRQHandling
*
* @brief		-
*
*  @param[in]	- SPI handle structure
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
void SPI_IRQHandling(SPI_Handle_t *pSPIHandle)
{
	uint32_t temp1, temp2;

	//first lets check for TXE
	temp1 = pSPIHandle->pSPIx->SR & (1 << SPI_SR_TXE);
	temp2 = pSPIHandle->pSPIx->CR2 & (1 << SPI_CR2_TXEIE);

	if(temp1 && temp2)
	{
		//handle TXE
		spi_txe_interrupt_handle(pSPIHandle); //handle data transmission
	}

	//check for RXNE
	temp1 = pSPIHandle->pSPIx->SR & (1 << SPI_SR_RXNE);
	temp2 = pSPIHandle->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);

	if(temp1 && temp2)
	{
		//handle RXNE
		spi_rxne_interrupt_handle(pSPIHandle); //handle data reception
	}

	temp1 = pSPIHandle->pSPIx->SR & (1 << SPI_SR_OVR);
	temp2 = pSPIHandle->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);

	if(temp1 && temp2)
	{
		//handle OVR
		spi_ovr_err_interrupt_handle(pSPIHandle); //handle overrun error
	}
}

/*******************************************************************************
* @fn		SPI_PeripheralControl
*
* @brief		- This function enables or disables the SPI peripheral
*
*  @param[in]	- base address of the gpio peripheral
*  @param[in]	- ENABLE or DISABLE macros
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	else
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
}

/*******************************************************************************
* @fn		SPI_SSIConfig
*
* @brief		- This function configures the SSI bit to avoid the Mode fault error (MODF)
*
*  @param[in]	- base address of the gpio peripheral
*  @param[in]	- ENABLE or DISABLE macros
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
		pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	else
		pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);
}


/*******************************************************************************
* @fn		SPI_SSOEConfig
*
* @brief		- This function configures the SSOE bit to avoid the Mode fault error (MODF)
*
*  @param[in]	- base address of the gpio peripheral
*  @param[in]	- ENABLE or DISABLE macros
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
		pSPIx->CR2 |= (1 << SPI_CR2_SSOE);
	else
		pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);
}

uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t flagName)
{
	if(pSPIx->SR & flagName)
	{
		return FLAG_SET;
	}

	return FLAG_RESET;
}

static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	// check the DFF bit in CR1
	if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
	{
		//16 bit DFF
		// load the data in to the DR
		pSPIHandle->pSPIx->DR = *((uint16_t*)pSPIHandle->pTxBuffer);
		pSPIHandle->TxLen--;
		pSPIHandle->TxLen--;
		(uint16_t*)pSPIHandle->pTxBuffer++;
	}
	else
	{
		//8 bit DFF
		pSPIHandle->pSPIx->DR = *pSPIHandle->pTxBuffer;
		pSPIHandle->TxLen--;
		pSPIHandle->pTxBuffer++;
	}

	if(! pSPIHandle->TxLen)
	{
		//if TxLen is 0, then close the SPI transmission and inform the application that transmission is over

		//this turn off the TXEIE interrupt
		SPI_CloseTransmission(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_CMPLT);

	}
}

static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	//2. check the DFF bit in CR1
	if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
	{
		//16 bit DFF
		//1. Read  the data from the DR to Rx buffer address
		*((uint16_t*)pSPIHandle->pRxBuffer) = (uint16_t)pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen--;
		pSPIHandle->RxLen--;
		(uint16_t*)pSPIHandle->pRxBuffer++;
	}
	else
	{
		//8 bit DFF
		*pSPIHandle->pRxBuffer = (uint8_t)pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen--;
		pSPIHandle->pRxBuffer++;
	}

	if(! pSPIHandle->RxLen)
	{
		//if TxLen is 0, then close the SPI reception and inform the application that reception is over

		//this turn off the RXNEIE interrupt
		SPI_CloseReception(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_CMPLT);

	}

}

static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	uint32_t temp;
	//1. clear the OVR flag
	if(pSPIHandle->TxLen != SPI_BUSY_IN_TX)
	{
		temp = pSPIHandle->pSPIx->DR;
		temp = pSPIHandle->pSPIx->SR;
	}
	(void)temp;

	//2. inform the application
	SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_OVR_ERR);
}


void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);
	pSPIHandle->pTxBuffer = NULL;
	pSPIHandle->TxLen = 0;
	pSPIHandle->TxState = SPI_READY;
}


void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);
	pSPIHandle->pRxBuffer = NULL;
	pSPIHandle->RxLen = 0;
	pSPIHandle->RxState = SPI_READY;
}


void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx)
{
	uint16_t temp;
	temp = (uint16_t)pSPIx->DR;
	temp = (uint16_t)pSPIx->SR;
	(void)temp;
}

__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEvt)
{
	//This is the weak implementation. The user application may override this function
}
