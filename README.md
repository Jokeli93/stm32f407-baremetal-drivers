**# STM32F407 Bare-Metal Driver Development (STM32F407)**



\## **Overview**



This project demonstrates the development of a register-level embedded driver stack for the STM32F407 microcontroller, implemented from scratch using bare-metal programming in C.



The goal of the project is to build a modular hardware abstraction layer (HAL-like driver architecture) without relying on vendor libraries such as HAL or LL.



All peripherals are controlled directly through memory-mapped registers based on the STM32F4 reference manual.



The project focuses on understanding:

* Microcontroller architecture
* Peripheral register programming
* Driver abstraction design
* Interrupt handling
* Embedded communication protocols



\---



**## Implemented Drivers**



\### GPIO

Implemented a full GPIO driver API with configurable parameters.



Features:

* Pin mode configuration (Input / Output / Alternate / Analog)
* Output type configuration (Push-Pull / Open-Drain)
* Speed configuration
* Pull-up / Pull-down configuration
* Pin read / write operations
* Port read / write operations
* Pin toggle functionality
* External interrupt configuration (EXTI)



Example use case implemented:



\#### External button interrupt controlling an LED

* Button press generates falling edge interrupt
* Interrupt handler toggles LED



This demonstrates:



* EXTI configuration
* NVIC interrupt handling
* Event-driven embedded software



\---



\### SPI Driver

Implemented a bare-metal SPI driver supporting master mode communication.



Features:



* SPI peripheral initialization
* Clock configuration
* Data transmission
* Full register-level control



\#### Test Setup

Communication between:



STM32F407 (Master)

Arduino Uno (Slave)



The STM32 sends data through SPI which is received and processed by the Arduino.



This validates:

* &#x20;SPI clock synchronization
* MOSI data transfer
* correct register configuration



\---



**## Driver Architecture**



The driver architecture follows a layered modular design.



Drivers/

&#x20;├── Inc/

&#x20;│    ├── stm32f407xx.h

&#x20;│    ├── stm32f407xx\_gpio\_driver.h

&#x20;│    ├── stm32f407xx\_spi\_driver.h

&#x20;│

&#x20;└── Src/

&#x20;     ├── stm32f407xx\_gpio\_driver.c

&#x20;     ├── stm32f407xx\_spi\_driver.c



\### Core MCU Header

stm32f407xx.h



Countains:

* Memory base addresses
* Bus peripheral addresses
* Peripheral register structures
* Clock control macros
* Generic macros (ENABLE/DISABLE)



\---



\### Driver Structure

Each driver follows a consistent structure:



\#### Header file

* Configuration structures
* Handle structures
* Driver APIs



\#### Source file

* Implementation of APIs
* Register configuration
* Interrupt support



\---



\## Example Applications



* Button interrupt toggling LED
* SPI data transmission between STM32F407 Microcontroller and Arduino



\### Technologies Used

* Embedded C
* ARM Cortex-M4
* STM32F407 microcontroller
* Bare-metal programming
* Register-level peripheral configuration
* Interrupt architecture
* SPI communication



\---



\## Future Development



This project will progressively implement additional drivers:



Planned drivers:

* USART Driver
* I2C Driver
* Advanced SPI features
* Timer Driver
* RTOS integration experiments
* DMA support



\---



\## Learning Goals



This project aims to develop deeper understanding of:

* microcontroller hardware architecture
* peripheral driver design
* interrupt-based embedded software
* communication protocol implementation
* clean driver abstraction



\---



**## Toolchain**



* STM32F407xx Discovery Board
* Arduino Uno
* STM32CubeIDE
* GCC ARM Embedded Toolchain
* Debugging via ST-Link
* Logic Analyzer



\---



\## Author



Joel Kevin Likane Zindjou



Junior Embedded Software Developer

Bachelor in Technical Computer Science



Focus areas:

* Embedded C / C++
* ARM Cortex-M
* Bare-metal programming
* Embedded communication protocols
* firmware architecture



**Note:**

This repository contains only the relevant source files.

Build artifacts and IDE-specific files are excluded using .gitignore.

