/*
 * driver_keypad.c
 *
 *  Created on: Jul 7, 2025
 *      Author: Korisnik
 */
#include "FreeRTOS.h"
#include "driver_keypad.h"
#include "gpio.h"
#include "driver_lcd.h"
#include "task.h"
#include "timers.h"
#include "driver_lcd.h"
#include "semphr.h"
#include <string.h>

static char KEY_Map[4][3] =
{
{ '1', '2', '3' },
{ '4', '5', '6' },
{ '7', '8', '9' },
{ '*', '0', '#' } };

static TaskHandle_t KEY_TaskHandle;
static TimerHandle_t KEY_TimerHandle;

char tempInputBuffer[3] =
{ '?', '?', '\0' }; //string
volatile uint32_t tempThreshold = 30;
uint8_t keyValueChanged = 0;
uint8_t tempIndex = 0;
static uint8_t KEY_PreviousReleased = 1;

static void KEY_Task(void *parameters)
{
	while (1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //waiting for a key to be pressed
		if (KEY_PreviousReleased)
		{
			//keypad scan
			for (uint8_t row = 0; row < 4; row++)
			{
				uint32_t gpiobOdr = GPIOB->ODR;
				GPIOB->ODR = (gpiobOdr & 0xFF00) | (0x01 << row);

				//reading the state from pins PB4, PB5, PB6 which are inputs to the OR circuit
				uint32_t state = (GPIOB->IDR >> 4) & 0x07;

				for (uint8_t column = 0; column < 3; column++)
				{
					if (state & (0x01 << column)) //the key is pressed
					{
						tempInputBuffer[tempIndex++] = KEY_Map[row][column];
						keyValueChanged = 1;

						if (tempIndex == 2 && tempInputBuffer[0] != '?'
								&& tempInputBuffer[1] != '?')
						{

							tempThreshold = (tempInputBuffer[0] - '0') * 10
									+ (tempInputBuffer[1] - '0');
							keyValueChanged = 1;
							tempIndex = 0;
							tempInputBuffer[0] = '?';
							tempInputBuffer[1] = '?';
						}
						KEY_PreviousReleased = 0; //the key is pressed, not released
						xTimerStart(KEY_TimerHandle, portMAX_DELAY);
					}
				}
			}
			GPIOB->ODR |= 0x0F; //A, B, C, D
		}
	}
}

static void KEY_TimerCallback(TimerHandle_t xTimer)
{
	if(KEY_PreviousReleased == 0) //key is pressed
	{
		uint32_t state = (GPIOB->IDR >> 4) & 0x07; //PB4, PB5, PB6
		if(state != 0)
		{
			//key is pressed for a longer time and this needs to be detected
			xTimerStart(KEY_TimerHandle, portMAX_DELAY);
		} else
		{
			KEY_PreviousReleased = 1; //key is released
		}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_7)
	{
		BaseType_t woken = pdFALSE;
		vTaskNotifyGiveFromISR(KEY_TaskHandle, &woken);
		portYIELD_FROM_ISR(woken);
	}
}

void KEY_Init()
{
	GPIOB->ODR = 0x0F; //A, B, C, D
	BaseType_t result = xTaskCreate(KEY_Task, "KEY_Task", 256, NULL, 6,
			&KEY_TaskHandle);
	if (result != pdPASS)
	{
		LCD_DisplayStringAtAddress(0x10, "Create Fail");
	}
	KEY_TimerHandle = xTimerCreate("KEY_Timer", pdMS_TO_TICKS(10), pdFALSE, NULL, KEY_TimerCallback);
	xTimerStart(KEY_TimerHandle, portMAX_DELAY);
}

