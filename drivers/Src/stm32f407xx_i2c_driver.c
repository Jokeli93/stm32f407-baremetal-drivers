/*
 * stm32f407xx_I2C_driver.c
 *
 *  Created on: 16.03.2026
 *      Author: Joelikane
 */

#include "stm32f407xx_I2C_driver.h"

uint16_t AHB_Prescaler[8] = {2, 4, 8, 16, 64, 128, 256, 512};

uint16_t APB1_Prescaler[8] = {2, 4, 8, 16};

uint32_t RCC_GetPLLOutputClock(void)
{
	//to implement if needed
	return 0;
}

uint32_t RCC_GetPCLK1Value(void)
{
	uint32_t pclk1, systemClk;
	uint8_t clkSrc, temp, ahbp, apb1p;

	clkSrc = ((RCC->CFGR >> 2) & 0x3);

	if(clkSrc == 0)
	{
		//HSI used as the system clock (16MHz)
		systemClk = 16000000;

	}
	else if(clkSrc == 1)
	{
		//HSE used as the system clock (8MHz)
		systemClk = 8000000;
	}
	else if(clkSrc == 2)
	{
		//PLL used as the system clock
		systemClk = RCC_GetPLLOutputClock();
	}

	//Bits 7:4 (HPRE) to configure the AHB prescaler
	temp = ((RCC->CFGR >> 4) & 0xF);

	if(temp < 8)
	{
		ahbp = 1;
	}
	else
	{
		ahbp = AHB_Prescaler[temp - 8]
	}

	//Bits 12:10 (PPRE1) to configure the APB1 prescaler
	temp = ((RCC->CFGR >> 10) & 0x7);

	if(temp < 4)
	{
		apb1p = 1;
	}
	else
	{
		apb1p = APB1_Prescaler[temp - 4];
	}

	pclk1 = (systemClk / ahbp) / apb1p;

	return pclk1;
}

//Peripheral Clock setup

/*******************************************************************************
* @fn			- GPIO_PeriClockControl
*
* @brief		- This function enables or disables peripheral clock for I2C
*
*  @param[in]	- base address of the gpio peripheral
*  @param[in]	- ENABLE or DISABLE macros
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pI2Cx == I2C1)
		{
			I2C1_PCLK_EN();
		}
		else if(pI2Cx == I2C2)
		{
			I2C2_PCLK_EN();
		}
		else if(pI2Cx == I2C3)
		{
			I2C3_PCLK_EN();
		}
	}
	else
	{
		if(pI2Cx == I2C1)
		{
			I2C1_PCLK_DI();
		}
		else if(pI2Cx == I2C2)
		{
			I2C2_PCLK_DI();
		}
		else if(pI2Cx == I2C3)
		{
			I2C3_PCLK_DI();
		}
	}
}

//Init and De-Init

/*******************************************************************************
* @fn			- I2C_Init
*
* @brief		- This function initializes/sets the given I2C peripheral
*
*  @param[in]	- Structure to handle a I2C peripheral
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
void I2C_Init(I2C_Handle_t *pI2CHandle)
{
	uint32_t tempReg =0;

	//enable the peripheral clock

	I2C_PeriClockControl(pI2CHandle->pI2Cx, ENABLE);

	//1. Configure the ACK control bit
	tempReg |= (pI2CHandle->I2C_Config.I2C_ACKControl << I2C_CR1_ACK);

	pI2CHandle->pI2Cx->CR1 = tempReg;

	//configure the FREQ field of CR2
	tempReg = 0;
	tempReg |= RCC_GetPCLK1Value() / 1000000U;

	pI2CHandle->pI2Cx->CR2 = (tempReg & 0x3F);

	//2. Programm the device own address
	tempReg |= pI2CHandle->I2C_Config.I2C_DeviceAddress << I2C_OAR1_ADD;
	tempReg |= (1 << 14); //bit 14 in I2C_OAR1 should always be kept as 1 by software

	pI2CHandle->pI2Cx->OAR1 = tempReg;

	//3. Configure speed and duty cycle

	//configure the CCR field
	uint16_t ccrValue = 0;
	tempReg =0;
	if(pI2CHandle->I2C_Config.I2C_SCLSpeed == I2C_SCL_SPEED_SM)
	{
		//standard mode
		ccrValue = RCC_GetPCLK1Value() /(2 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
		tempReg |= (ccrValue & 0xFFF);
	}
	else
	{
		//fast mode
		tempReg |= (1 << I2C_CCR_FS); // first set the mode to fast mode

		tempReg |= pI2CHandle->I2C_Config.I2C_FMDutyDycle << I2C_CCR_DUTY;

		if(pI2CHandle->I2C_Config.I2C_FMDutyDycle == I2C_FM_DUTY_2)
		{
			ccrValue = RCC_GetPCLK1Value() /(3 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
		}
		else
		{
			ccrValue = RCC_GetPCLK1Value() /(25 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
		}
		tempReg |= (ccrValue & 0xFFF);
	}

	pI2CHandle->pI2Cx->CCR = tempReg;
}

/*******************************************************************************
* @fn			- I2C_DeInit
*
* @brief		- This function resets the given I2C peripheral
*
*  @param[in]	- base address of the gpio peripheral
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{
	if(pI2Cx == I2C1)
		{
			I2C1_REG_RESET();
		}
		else if(pI2Cx == I2C2)
		{
			I2C2_REG_RESET();
		}
		else if(pI2Cx == I2C3)
		{
			I2C3_REG_RESET();
		}
}

//IRQ configuration and ISR handling

/*******************************************************************************
* @fn			- I2C_IRQInterruptConfig
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
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
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
* @fn			- I2C_IRQPriorityConfig
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
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_section) + (8- NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}

/*******************************************************************************
* @fn		I2C_PeripheralControl
*
* @brief		- This function enables or disables the I2C peripheral
*
*  @param[in]	- base address of the gpio peripheral
*  @param[in]	- ENABLE or DISABLE macros
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
		pI2Cx->CR1 |= (1 << I2C_CR1_PE);
	else
		pI2Cx->CR1 &= ~(1 << I2C_CR1_PE);
}

uint8_t I2C_GetFlagStatusSR1(I2C_RegDef_t *pI2Cx, uint32_t flagName)
{
	if(pI2Cx->SR1 & flagName)
	{
		return FLAG_SET;
	}

	return FLAG_RESET;
}

uint8_t I2C_GetFlagStatusSR2(I2C_RegDef_t *pI2Cx, uint32_t flagName)
{
	if(pI2Cx->SR2 & flagName)
	{
		return FLAG_SET;
	}

	return FLAG_RESET;
}
