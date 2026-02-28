/*
 * stm32f407xx_spi_drver.h
 *
 *  Created on: 28.02.2026
 *      Author: Joelikane
 */

#ifndef INC_STM32F407XX_SPI_DRIVER_H_
#define INC_STM32F407XX_SPI_DRIVER_H_

#include "stm32f407xx.h"

//Configuration structure for SPIx peripheral

typedef struct
{
	uint8_t SPI_DeviceMode;
	uint8_t SPI_BusConfig;
	uint8_t SPI_SclkSpeed;
	uint8_t SPI_DFF;
	uint8_t SPI_CPOL;
	uint8_t SPI_CPHA;
	uint8_t SPI_SSM;

}SPI_Config_t;

//Handle structure for SPIx peripheral

typedef struct
{
	SPI_RegDef_t * pSPIx;		//This holds the base address of SPIx(x: 1, 2, 3, 4, 5, 6)
	SPI_Config_t SPIConfig;

}SPI_Handle_t;


/**********************************************************************************
 *							APIs supported by this driver
 *		For more information about the APIs check the function definitions
**********************************************************************************/

//Peripheral Clock setup
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);

//Init and De-Init
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);

//Data send and receive
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t len);
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t len);

//IRQ configuration and ISR handling
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi); // used to configure the IRQ number of the SPI
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority); // Function to configure the IRQ priority
void SPI_IRQHandling(SPI_Handle_t *pSPIHandle); // Function to process interrupts


#endif /* INC_STM32F407XX_SPI_DRIVER_H_ */
