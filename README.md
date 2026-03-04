**# STM32F407 Bare-Metal Driver Development**



\## **Overview**

This project contains a fully custom register-level driver stack for the STM32F407xx microcontroller using bare metal programming in C.



**## Learning Objectives**

* Develop production-style driver architecture
* Understand hardware-software interaction at register level
* Practice modular, reusable embedded code design
* Prepare for advanced topics (RTOS, middleware, communication stacks)



**## Implemented Drivers**

* GPIO (polling and interrupt based mode)
* Peripheral clock control
* SPI (polling  and interrupt based)



**## Architecture**

* MCU register abstraction layer
* Peripheral driver interface layer
* Peripheral implementation layer



**## Project Structure**



* **stm32f407xx.h**



&#x09;- Memory base addresses (Flash, SRAM, ROM)



&#x09;- Bus base addresses (AHB1, APB1, AHB2, APB2)



&#x09;- Peripheral base addresses (GPIO, RCC, USART, SPI, I2C, etc.)



&#x09;- Register definition structures



&#x09;- Clock enable/disable macros



* **stm32f407xx\_gpio\_driver.h**



&#x09;- GPIO configuration structures



&#x09;- API prototypes



* **stm32f407xx\_spi\_driver.h**



&#x09;- SPI configuration structures



&#x09;- API prototypes



* **stm32f407xx\_gpio\_driver.c**



&#x09;- Implementation of GPIO initialization



&#x09;- Clock control



&#x09;- Read/write/toggle functions



* **stm32f407xx\_spi\_driver.c**



**Core Files**

drivers/

&#x20;├── Inc/

&#x20;│    ├── stm32f407xx.h

&#x20;│    └── stm32f407xx\_gpio\_driver.h

&#x20;└── Src/

&#x20;     └── stm32f407xx\_gpio\_driver.c



**## Design Decisions**

* No HAL usage
* Direct register manipulation
* Bit-masking configuration
* Handle-based driver abstraction



**## Toolchain**



* STM32F407xx Discovery Board
* STM32CubeIDE
* GCC ARM Embedded Toolchain
* Debugging via ST-Link
* Logic Analyzer



**## Planned Features**

* EXTI interrupt support
* USART driver (polling + interrupt)
* I2C driver
* NVIC abstraction layer



**Note:**

This repository contains only the relevant source files.

Build artifacts and IDE-specific files are excluded using .gitignore.

