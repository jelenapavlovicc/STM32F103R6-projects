## Project Overview

This project simulates a weather monitoring system based on the STM32F103R6 microcontroller. The project has been built using the ARM GCC toolchain (arm-none-eabi), Eclipse CDT, STM32CubeMX, and the Proteus simulator.
The system acquires environmental data—temperature, wind speed, and wind direction—and displays the values in real time on both a character LCD and a virtual terminal. An alert mechanism, based on a red LED indicator, is included to signal high wind speeds.

## Features

- Temperature monitoring using the LM35 sensor (operating range: 0–60 °C)
- Wind speed measurement using an anemometer (based on PWM signal processing)
- Wind direction detection using a wind vane (azimuth determined from analog voltage)
- Real-time display on a 4-line LCD (LM041L, HD44780-compatible)
- Output to a virtual terminal over USART1
- LED alarm: activated blinking when wind speed is equal to or exceeds 50 km/h
- Data refresh every 200 milliseconds on both the LCD and virtual terminal when changes occur

LCD Output:
```
Azimuth: <azimuth>
Wind speed: <windspeed>
Temper: <temper>
Duty cycle: <value>
```

Virtuel Terminal Output:
```
<azimuth>/<windspeed>/<temper>
```

## System Arhcitecture 

- Hardware:
    - Microcontroller: STM32F103R6 (Cortex-M3)
    - Analog Multiplexer: 74HC4051 (analog channel selection)
    - Sensors:
        - LM35 temperature sensor (connected to X0 on the multiplexer)
        - Wind vane (connected to X1 on the multiplexer)
        - Anemometer (connected to TIM1 channels CH1 and CH2)
    - Display: 
		- LM041L LCD (4-line character display, HD44780 compatible)
    - Communication: Virtual terminal via USART1 (PB6/PB7)


## Key Algorithms

- Wind speed calculation: `speed = frequency × 2.4 km/h`
- Wind direction mapping: The analog voltage from the wind vane is mapped to a discrete azimuth value based on a predefined voltage-to-angle conversion function.
- Temperature conversion: `temperature [°C] = (ADC_value × V_REF × 100) / ADC_resolution`

## Simulation Notes

The anemometer outputs a PWM signal whose frequency is directly proportional to wind speedWind vane provides analog voltage corresponding to wind direction.
The wind vane provides an analog voltage corresponding to the wind direction (azimuth)
LED blinks at 1Hz when wind speed is ≥ 50 km/h.
All displays are refreshed only when a change in the corresponding measurement is detected, minimizing unnecessary updates.
	
## Schematic and Video Demonstration

The schematic design of the project is included in the `Schematic_Design.pdf` file. A video demonstrating the project is available in the `stm32f103r6_weather_monitoring.mp4 file`.

## Code Structure

The following modules and functionalities have been implemented as part of this project:

- `...\cubemx1\code`:

	- `uart_driver.h`, `uart_driver.c` - A custom driver for the USART peripheral has been developed to support transmission functionalities for asynchronous communication between the MCU and the virtual terminal.

	The following functions have been provided:

		- `void UART_AsyncTxString(UART_Target target, char const * string)`
		- `void UART_AsyncTxCharacter(uint8_t buffer)`
		
	- `lcd.h`, `lcd.c` - This module provides complete logic for LCD control and interfacing. A 4-bit data interface has been used to communicate with the HD44780-compatible display. Characters are rendered using a 5×8 dot matrix format.
	
	- `sensors.h`, `sensors.c` - The logic for reading values from the sensors has been implemented, including the initiation of ADC conversions and processing of the acquired data.
	
	- `exam.h`, `exam.c` - Functionality for displaying sensor values on both the LCD and the virtual terminal has been implemented. Additionally, LED blinking behavior has been handled for wind speeds exceeding 50 km/h.

	- `string_utils.h`, `string_utils.c` - Provides utility functions for converting numeric values into formatted, right-aligned strings suitable for display on the LCD.

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
