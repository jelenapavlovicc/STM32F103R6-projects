/*
 * work.c
 *
 *  Created on: Aug 24, 2025
 *      Author: Korisnik
 */


#include "FreeRTOS.h"
#include "task.h"

#include <string.h>
#include <stdlib.h>
#include "work.h"
#include "driver_uart.h"
#include "driver_temp.h"
#include "driver_lcd.h"

FanState fanState = TURNED_OFF;

static uint32_t tempValue;
static uint32_t tempValueDisplay;
static char tempText[4];

static void work_Task(void* parameters)
{
	char message[16] = "Temperatura: ";
	LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_INSTRUCTION_SET_DDRAM_ADDR | 0x00);
	for(uint32_t i = 0; i < 16; i++)
	{
		LCD_CommandEnqueue(LCD_DATA, message[i]);
		UART_AsyncTxCharacter(message[i]);
	}

	while(1)
	{
		tempValue = TEMP_GetTemperatureValue();

		//control motor
		FanState fanStateTarget;
		if(tempValue < 30)
		{
			fanStateTarget = TURNED_OFF;
		} else if(tempValue < 35)
		{
			fanStateTarget = SLOW;
		} else
		{
			fanStateTarget = FAST;
		}

		for(uint32_t i = 0; i < abs(fanStateTarget - fanState); i++)
		{
			if(fanStateTarget > fanState)
			{
				MOTOR_SpeedIncrease();
			} else
			{
				MOTOR_SpeedDecrease();
			}
		}
		fanState = fanStateTarget;

		if(tempValue > 60)
		{
			tempValueDisplay = 60;
		} else if(tempValue < 0)
		{
			tempValueDisplay = 0;
		} else {
			tempValueDisplay = tempValue;
		}

		//Write temperature to Virtuel terminal and LCD display
		itoa(tempValueDisplay, tempText, 10);
		LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_INSTRUCTION_SET_DDRAM_ADDR | 0x0D);
		for(uint32_t i = 0; i < strlen(tempText); i++)
		{
			LCD_CommandEnqueue(LCD_DATA, tempText[i]);
			UART_AsyncTxCharacter(tempText[i]);
		}

		vTaskDelay(pdMS_TO_TICKS(200));

		//clear temperature value
		LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_INSTRUCTION_SET_DDRAM_ADDR | 0x0D);
		for(uint32_t i = 0; i < strlen(tempText); i++)
		{
			LCD_CommandEnqueue(LCD_DATA, ' ');
			UART_AsyncTxCharacter('\b');
		}
	}
}

void work_Init()
{
	TEMP_Init();
	UART_Init();
	LCD_Init();
	MOTOR_Init();
	xTaskCreate(work_Task, "work_Task", 64, NULL, 5, NULL);
}
