## STM32F103R6 Microcontroller Projects Repository

## Overview

This repository contains a collection of embedded systems projects implemented on the STM32F103R6 microcontroller. Each projest has been implemented using the arm-none-eabi toolchain, build tools, Eclipse CDT, the Proteus simulator and CubeMX.

## Required Tools

Following toolchain components are required:

- **GNU MCU Eclipse IDE (version 2021-06-R)**
- **GNU ARM Toolchain (version 10.2.1)**
- **GNU MCU Build Tools (version 4.2.1-2)**

These tools can be downloaded from Google Drive:

- [Download tools.zip from Google Drive](https://drive.google.com/drive/folders/1CaS04-ndH8tqtUvtaRrHiHTAvh6bvUi4?usp=drive_link)

## Weather Monitoring System

Directory: STM32F103R6_temperature_azimuth_wind_speed
Description:
An environmental monitoring system capable of:
- Measures ambient temperature using LM35 sensor
- Detects wind direction using analog wind vane
- Calculates wind speed through anemometer frequency analysis
- Displays real-time data on 4-line LCD display and Virtuel terminal
- Activating an LED blinking alarm when wind speed exceeds a defined threshold

## Seven-Segment Display Timer

Directory: STM32F103R6_seven_segment_display
Description:
A digital timer implementation featuring:
- 4-digit seven-segment display (MM:SS format)
- Count range: 00:00 to 59:59
- Manual reset capability

## PWM Motor/LED Control System

Directory: STM32F103R6_pwm_control_motor_led
Description:
A dual-purpose PWM control system for:
- Regulating DC motor speed
- Adjusting LED brightness
Control is achieved with two push-buttons for incrementing and decrementing PWM duty cycle.

## Dual-MCU Motor Control System

Directory: STM32F103R6_dual_mcu_dual_motor_uart_lcd
Description:
A control system based featuring:
- Two STM32F103R6 microcontrollers
- Two independently-controlled DC motors
- MCU1 receives commands via virtual terminal (USART protocol) and transmit instructions to MCU2 (USART protocol)
- MCU2 executes motor speed adjustments 
- LCD display shows motor 1 speed (Line 1) and motor 2 speed (Line 2)

## Temperature and Rain Monitoring and Fan Control System

Directory: STM32F103R6_temperature_rain_monitoring_fan_control  
Description:  
- Ambient temperature monitoring via LM35 sensor (ADC-based)  
- Rainfall intensity measurement using tipping-bucket rain gauge and monostable pulse shaping  
- User-configurable temperature threshold input via matrix keypad  
- Fan control using PWM based on temperature thresholds  
- Bi-color LED behavior indicating safe or alert temperature states  
- Real-time data display on both LCD and virtual terminal

## Temperature Fan Control System

Directory: STM32F103R6_temperature_fan_control
Description:
- Ambient temperature monitoring via LM35 sensor (ADC-based) 
- Fan control using PWM based on temperature thresholds  
- Real-time data display on both LCD and virtual terminal
