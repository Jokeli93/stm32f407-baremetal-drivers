/*
 * stm32f407xx_rcc_driver.c
 *
 *  Created on: 07.04.2026
 *      Author: Joelikane
 */

#include "stm32f407xx_rcc_driver.h"

uint16_t AHB_Prescaler[8] = {2, 4, 8, 16, 64, 128, 256, 512};

uint16_t APB1_Prescaler[8] = {2, 4, 8, 16};


/*******************************************************************************
* @fn			- RCC_GetPCLK1Value
*
* @brief		- This function returns the APB1 clock value
*
*  @param[in]	-
*  @param[in]	-
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
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
		ahbp = AHB_Prescaler[temp - 8];
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

/*******************************************************************************
* @fn			- RCC_GetPCLK2Value
*
* @brief		- This function returns the APB2 clock value
*
*  @param[in]	-
*  @param[in]	-
*
* @return		- none
*
* @note			- none
*
********************************************************************************/
uint32_t RCC_GetPCLK2Value(void)
{
	uint32_t SystemClock=0,tmp,pclk2;
	uint8_t clk_src = ( RCC->CFGR >> 2) & 0X3;

	uint8_t ahbp,apb2p;

	if(clk_src == 0)
	{
		SystemClock = 16000000;
	}else
	{
		SystemClock = 8000000;
	}
	tmp = (RCC->CFGR >> 4 ) & 0xF;

	if(tmp < 0x08)
	{
		ahbp = 1;
	}else
	{
       ahbp = AHB_Prescaler[tmp-8];
	}

	tmp = (RCC->CFGR >> 13 ) & 0x7;
	if(tmp < 0x04)
	{
		apb2p = 1;
	}else
	{
		apb2p = APB1_Prescaler[tmp-4];
	}

	pclk2 = (SystemClock / ahbp )/ apb2p;

	return pclk2;
}


uint32_t RCC_GetPLLOutputClock(void)
{
	//to implement if needed
	return 0;
}
