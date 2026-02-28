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

