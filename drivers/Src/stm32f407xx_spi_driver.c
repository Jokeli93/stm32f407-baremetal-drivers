/*
 * stm32f407xx_spi_driver.c
 *
 *  Created on: 28.02.2026
 *      Author: Joelikane
 */

#include "stm32f407xx_spi_driver.h"


//Peripheral Clock setup

/*******************************************************************************
* @fn			- GPIO_PeriClockControl
*
* @brief		- This function enables or disables peripheral clock for the given GPIO port
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


uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t flagName)
{
	if(pSPIx->SR & flagName)
	{
		return FLAG_SET;
	}

	return FLAG_RESET;
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
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t len)
{

}

//IRQ configuration and ISR handling

/*******************************************************************************
* @fn			- SPI_IRQInterruptConfig
*
* @brief		-This function enables/disable the interrupt of  a given IRQ number
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
