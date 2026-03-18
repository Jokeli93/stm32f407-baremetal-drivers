/*
 * stm32f407xx_i2c_driver.h
 *
 *  Created on: 16.03.2026
 *      Author: Joelikane
 */

#ifndef INC_STM32F407XX_I2C_DRIVER_H_
#define INC_STM32F407XX_I2C_DRIVER_H_

#include "stm32f407xx.h"


//Configuration structure of the I2Cx peripheral

typedef struct
{
	uint32_t I2C_SCLSpeed; //Clock speed
	uint8_t  I2C_DeviceAddress; //Device address for Slave devices
	uint8_t  I2C_ACKControl; //ACK or NACK
	uint16_t I2C_FMDutyDycle; //Duty cycle
}I2C_Config_t;

//Handle structure of I2Cx peripheral

typedef struct
{
	I2C_RegDef_t * pI2Cx;
	I2C_Config_t I2C_Config;
}I2C_Handle_t;

//@I2C_SCLSpeed
#define I2C_SCL_SPEED_SM		100000 //Simple Mode
#define I2C_SCL_SPEED_FM2K		200000 //Fast Mode 200KHz
#define I2C_SCL_SPEED_FM4K		400000 //Fast Mode 400KHz

//@I2C_ACKControl
#define I2C_ACK_ENABLE		1
#define I2C_ACK_DISABLE		0

//@I2C_FMDutyDycle
#define I2C_FM_DUTY_2		0
#define I2C_FM_DUTY_16_9	1

//I2C related status flags definitions
#define I2C_FLAG_SB				(1 << I2C_SR1_SB)
#define I2C_FLAG_ADDR			(1 << I2C_SR1_ADDR)
#define I2C_FLAG_BTF			(1 << I2C_SR1_BTF)
#define I2C_FLAG_STOPF			(1 << I2C_SR1_STOPF)
#define I2C_FLAG_RXNE			(1 << I2C_SR1_RXNE)
#define I2C_FLAG_TXE			(1 << I2C_SR1_TXE)
#define I2C_FLAG_BERR			(1 << I2C_SR1_BERR)
#define I2C_FLAG_ARLO			(1 << I2C_SR1_ARLO)
#define I2C_FLAG_AF				(1 << I2C_SR1_AF)
#define I2C_FLAG_OVR			(1 << I2C_SR1_OVR)
#define I2C_FLAG_PECERR			(1 << I2C_SR1_PECERR)
#define I2C_FLAG_TIMEOUT		(1 << I2C_SR1_TIMEOUT)
#define I2C_FLAG_SMBALERT		(1 << I2C_SR1_SMBALERT)

/**********************************************************************************
 *							APIs supported by this driver
 *		For more information about the APIs check the function definitions
**********************************************************************************/

//Peripheral Clock setup
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi);

//Init and De-Init
void I2C_Init(I2C_Handle_t *pI2CHandle);
void I2C_DeInit(I2C_RegDef_t *pI2Cx);

//Data send and receive
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t len, uint8_t slaveAddr);

//IRQ configuration and ISR handling
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi); // used to configure the IRQ number of the I2C
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority); // Function to configure the IRQ priority



// enables or disables the I2C peripheral
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi);

//return the flag status of the Status Register
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t flagName);
//uint8_t I2C_GetFlagStatusSR2(I2C_RegDef_t *pI2Cx, uint32_t flagName);


//Application callback
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEvt);


#endif /* INC_STM32F407XX_I2C_DRIVER_H_ */
