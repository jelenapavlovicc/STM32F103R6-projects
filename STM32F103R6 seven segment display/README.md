## Project Overview

This project is designed for the STM32F103R6 microcontroller and was built using the arm-none-eabi tools, build tools, Eclipse CDT, the Proteus simulator, and CubeMX. The following tasks have been implemented:

- **GPIO Configuration:**  
  Pins PB12-PB15 are configured as general-purpose output pins and are connected to LEDs in Proteus. The function `void setGPIOB(uint16_t GPIO_PIN, GPIO_PinState GPIO_PIN_STATE)` is implemented using the HAL library to set the states of these pins on Port B.

- **External Interrupt:**  
  Pin PB11 is configured to generate interrupts when a falling edge is detected on the input signal of this pin. In Proteus, a pushbutton is connected to this pin with a pull-down resistor. Inside the interrupt routine, the state of pin PB12 is toggled.
  
- **Stopwatch Implementation:**  
  In Proteus, a seven-segment display is connected to the STM32F103R6. A stopwatch is implemented that displays the number of minutes on the two upper digits and the number of seconds on the two lower digits of the display.
  
- **Interrupt for Stopwatch Reset:**  
  The button is implemented so that when it is pressed, it resets the stopwatch.

- **Frequency Calculation and Display:**  
  The frequency of the clock generator signal fed to pin PA8 is calculated. In Proteus, the clock generator has a period of 2 seconds. When the button is pressed and held in the pressed state, the frequency of the clock generator signal is displayed on the seven-segment display in mHz.

- **LED Blinking:**  
  An LED is connected to pin PA6, and it is alternately turned off for 0.5 seconds and on for 1.5 seconds, without directly manipulating the pin in the code.
  
## Schematic and Video Demonstration

The schematic design of the project can be found in the **Schematic_Design.pdf** file. Additionally, a video demonstrating the project is available in the **stm32f103r6_with_seven_segment_display.mp4** file.

## Code Structure

The main parts of the code that I worked on are located in the following files:
- **gpio.c:** Contains the `setGPIOB()` function.
- **stm32f1xx_it.c:** Includes the overridden callbacks for interrupt routines.
- **display_timer.c, display_timer.h:** Handles the logic for the seven-segment display.

The rest of the code was automatically generated using CubeMX.

## Required Tools

This project requires the following tools:

- **GNU MCU Eclipse IDE** (version 2021-06-R)
- **GNU ARM Toolchain** (version 10.2.1)
- **GNU MCU Build Tools** (version 4.2.1-2)

These tools can be downloaded from Google Drive:

- [Download tools.zip from Google Drive](https://drive.google.com/drive/folders/1CaS04-ndH8tqtUvtaRrHiHTAvh6bvUi4?usp=drive_link)

## Installation Steps

- Extract the archives containing the required tools.
- Add the following directories to the system's PATH environment variable:
	- **build-tools-4.2.1-2\bin**
	- **arm-none-eabi-gcc-10.2.1-1.1\bin**


## Running the Project
- Proteus:
	Run as Administrator: Open Proteus with administrator rights.
    Open proteus project. 
	Program File for STM32F103R6: Choose one of the following program files:
		- **...\build\code.elf**
		- **...\build\code.hex**
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

