## Project Overview 

This project is developed for the STM32F103R6 microcontroller and was built using the arm-none-eabi tools, build tools, Eclipse CDT, the Proteus simulator, and CubeMX. It demonstrates the usage of a single PWM (Pulse Width Modulation) signal to control both the speed of a motor and the brightness of an LED. 
This single PWM signal controls both:
    - **Motor speed**.
    - **LED brightness**.
Additionally, an oscilloscope is implemented in the system to monitor the PWM signal. 


## Schematic and Video Demonstration

The schematic design of the project can be found in the **Schematic_Design.pdf** file. Additionally, a video demonstrating the project is available in the **stm32f103r6_pwm_control_for_motor_speed_and_led_brightness.mp4** file.

## Instructions 
- Press the Increase Button (INC) to speed up the motor and increase the LED brightness.
- Press the Decrease Button (DEC) to slow down the motor and decrease the LED brightness.
	
## Code Structure

The main parts of the code that I worked on are located in the following files:
- **stm32f1xx_it.c:** Includes the overridden callbacks for interrupt routines.

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
    Open proteus project file. 
	For the STM32F103R6 program file, choose one of the following options:
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
