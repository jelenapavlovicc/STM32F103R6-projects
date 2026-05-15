## Project Overview

This project simulates a temperature monitoring system based on the STM32F103R6 microcontroller. The project has been built using the ARM GCC toolchain (arm-none-eabi), Eclipse CDT, STM32CubeMX, and the Proteus simulator.
The system acquires environmental data temperature and displays the value in real time on both a character LCD and a virtual terminal. Additionally, a cooling fan rotates at a speed proportional to the temperature.

## Features

- Temperature monitoring using the LM35 sensor (operating range: 0–60 °C)
- Real-time display on a LCD (LM020L, HD44780-compatible)
- Output to a virtual terminal over USART1
- Data refresh every 200 milliseconds on both the LCD and virtual terminal when changes occur
- Cooling fan remains off whe the temperature is below 30°C, rotates at 50% of its maximum speed when the temperature is between 30°C and 35°C, and operates at full speed when the temperature exceeds 35°C.

LCD Output:
```
Temperatura: <temper>
```

Virtuel Terminal Output:
```
Temperatura: <temper>
```

## System Arhcitecture 

- Hardware:
    - Microcontroller: STM32F103R6 (Cortex-M3)
    - Sensors:
        - LM35 temperature sensor 
    - Display: 
		-  LM020L LCD (HD44780 compatible)
    - Communication: Virtual terminal via USART1 
	- Actuator:
		- DC Motor (for fan control implementation)

## Key Algorithms

- Temperature conversion: `temperature [°C] = (ADC_value × V_REF × 100) / ADC_resolution`

## Schematic and Video Demonstration

The schematic design of the project is included in the `Schematic_Design.pdf` file. A video demonstrating the project is available in the `stm32f103r6_temperature_fan_control.mp4 file`.

## Code Structure

The following modules and functionalities have been implemented as part of this project:

- `...\cubemx\code`:

	- `driver_uart.h`, `driver_uart.c` - A custom driver for the USART peripheral has been developed to support transmission functionalities for asynchronous communication between the MCU and the virtual terminal.

	- `driver_lcd.h`, `driver_lcd.c` - This module provides complete logic for LCD control and interfacing. A 4-bit data interface has been used to communicate with the HD44780-compatible display. Characters are rendered using a 5×8 dot matrix format.
	
	- `driver_temp.h`, `driver_temp.c` - The logic for reading values from the temperature sensor has been implemented, including the initiation of ADC conversion and processing of the acquired data.
	
	- `driver_motor.h`, `driver_motor.c` - This module implements PWM-based control for the DC motor (fan) with adaptive speed regulation based on temperature thresholds. The driver provides functions for motor initialization, speed control (0%, 50%, 100% duty cycle), and implements the fan control logic.
	
	- `work.h`, `work.c` - Functionality for displaying temperature sensor value on both the LCD and the virtual terminal has been implemented. 

The rest of the code was automatically generated using CubeMX.

## Required Tools

This project requires the following tools:

- **GNU MCU Eclipse IDE** (version 2021-06-R)
- **GNU ARM Toolchain** (version 10.2.1)
- **GNU MCU Build Tools** (version 4.2.1-2)

## Installation Steps

- Extract the archives containing the required tools.
- Add the following directories to the system's PATH environment variable:
	- **build-tools-4.2.1-2\bin**
	- **arm-none-eabi-gcc-10.2.1-1.1\bin**

## Running the Project

- Eclipse CDT:
	- Workspace Setup: 
		- Set the workspace to the **cubemx** folder.
	- Toolchain Setup:
		- Navigate to **Windows -> Preferences -> Workspace -> Arm Toolchains Paths**.
        - Set the Default Toolchain to: **xPack FNU Arm Embedded GCC**
        - Set the Toolchain Folder to: **...\arm-none-eabi-gcc-10.2.1-1.1\bin**
        - Set the Build Tools Folder to: **...\build-tools-4.2.1-2\bin**
	- Importing the Project:
		- Select **Import -> C/C++ -> Existing Code as Makefile Project**.
        - Set the Existing Code Location to **...\project\cubemx\code**.
        - Select the Toolchain: **Arm Cross GCC**.
	- Project Settings:
		- Navigate to **Properties -> C/C++ Build -> Settings** and click **Apply and Close** (wait for the indexer to process paths).
		- Navigate to **Properties -> C/C++ General -> Paths and Symbols -> Symbols**  and add the following symbols as GNU C:
			- **USE_HAL_DRIVER**
			- **STM32F103x6**
	- Creating Build Targets:
		- Right-click on the project and select **Build Targets -> Create**.
		- Create the following targets: 
			- **all**
			- **clean**
	- Building the Project:
		- Double-click the **all** target to build the project.
	
- Proteus:
Run as Administrator: Open Proteus with administrator rights.
Open proteus project file. 
For the STM32F103R6 program file, choose one of the following options:
	- **...\build\code.elf**
	- **...\build\code.hex**
