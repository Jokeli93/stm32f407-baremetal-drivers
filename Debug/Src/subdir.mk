################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/014i2c_slave_tx_string2.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/014i2c_slave_tx_string2.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/014i2c_slave_tx_string2.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -I"C:/Users/Joelikane/Desktop/Embedded_C_programming_coursera/Mastering_Microcontroller_Embedded_Driver_Development/MCU1_Course_Workspace/target/stm32f4xx_drivers/drivers/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/014i2c_slave_tx_string2.cyclo ./Src/014i2c_slave_tx_string2.d ./Src/014i2c_slave_tx_string2.o ./Src/014i2c_slave_tx_string2.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su

.PHONY: clean-Src

