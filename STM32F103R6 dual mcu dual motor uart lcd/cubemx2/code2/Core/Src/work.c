/*
 * work.c
 *
 *  Created on: May 30, 2025
 *      Author: Korisnik
 */


#include "work.h"

#include "FreeRTOS.h"
#include "task.h"
#include "uart_driver.h"
#include "lcd.h"

#include "tim.h"


static void workTask(void* parameters)
{

	//new character code is 0x01
	LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_INSTRUCTION_SET_CGRAM_ADDRESS | 0x08);

	//pattern
	LCD_CommandEnqueue(LCD_DATA, 0x1F);
	LCD_CommandEnqueue(LCD_DATA, 0x1F);
	LCD_CommandEnqueue(LCD_DATA, 0x1F);
	LCD_CommandEnqueue(LCD_DATA, 0x1F);
	LCD_CommandEnqueue(LCD_DATA, 0x1F);
	LCD_CommandEnqueue(LCD_DATA, 0x1F);
	LCD_CommandEnqueue(LCD_DATA, 0x1F);


	//initialization of TIM3 using Channel 1 and Channel 2 to control the velocity of MOTOR1 and MOTOR2.
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

	//initially the speeds of the motors are 0
	htim3.Instance->CCR1 = 0; //for Channel 1
	htim3.Instance->CCR2 = 0; //for Channel 2

	while(1)
	{

		//receive from MCU1
		MotorCommand motorCommand = UART_BlockRxMotorCommand();

		switch (motorCommand.motor)
		{
			case 1:
				htim3.Instance->CCR1 = motorCommand.velocity;
				LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_INSTRUCTION_SET_DDRAM_ADDRESS | 0x00);
				break;
			case 2:
				htim3.Instance->CCR2 = motorCommand.velocity;
				LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_INSTRUCTION_SET_DDRAM_ADDRESS | 0x40);
				break;
		}

		//display on LCD display
		for(uint32_t i = 0; i < 16; i++)
		{
			if(i < motorCommand.velocity)
			{
				LCD_CommandEnqueue(LCD_DATA, 0x01);
			} else
			{
				LCD_CommandEnqueue(LCD_DATA, ' ');
			}
		}


	}
}

void workInit()
{
	UART_Init();
	LCD_Init();
	xTaskCreate(workTask, "workTask", 128, NULL, 5, NULL);
}
