/*
 * work.c
 *
 *  Created on: Jun 27, 2025
 *      Author: Korisnik
 */

#include "FreeRTOS.h"
#include "work.h"
#include "driver_uart.h"
#include "driver_temper.h"
#include "driver_lcd.h"
#include "driver_keypad.h"
#include "driver_motor.h"
#include "task.h"
#include "timers.h"
#include "tim.h"
#include "gpio.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ARR 				65535
#define FREQUENCY_CLK_TIM3	8000000

TaskHandle_t WORK_TaskHandle;
TimerHandle_t LED_TimerHandle;

static uint8_t valueChangedFlagVT = 0;
static volatile uint8_t valueChangedFlagLCD = 1;

//temperature
static char *temperatureString = "Temp: ";
static float temperatureValue = -2.0;
static float prevTemperautreValue = -1.0;
static uint32_t temperatureToDisplay = -1;
static uint32_t prevLenTemperatureString = 0;

//rain
static char *rainfallString = "Rain: ";
static uint32_t elapsedMs = 0;
static uint32_t rainfallMmPerHour = 0;
uint16_t volatile timestamptStart = 0;
uint16_t volatile timestamptEnd = 0;
uint32_t volatile tickElapsedPeriod = 0;
uint32_t volatile counterOverflowCounter = 0;

//temperature threshold
static char *thresholdTempString1 = "Thld:(";
static char *thresholdTempString2 = ")=>";
char digitStr1[2];
char digitStr2[2];
uint32_t currentThreshold;

//motor
FanState fanState = TURNED_OFF; //initial fan is turned off

typedef enum
{
	WAIT_INITIAL_EDGE = 0, WAIT_PERIOD_END
} PeriodStateMachine;

PeriodStateMachine state = WAIT_INITIAL_EDGE;

//When using the function, it is necessary to free the space occupied by
//the string with the vPortFree function
char* intToString(uint32_t number)
{

	char digits[32]; //0,1,...31
	uint32_t index = 32; //form digits from tha least significant digits
	char *string = pvPortMalloc(32 + 1); //32 digits and 1 for '\0'
	if (string == NULL)
		return NULL;

	if (number == 0)
	{
		string[0] = '0';
		string[1] = '0';
		string[2] = '\0';
	}
	else if (number < 10)
	{
		string[0] = '0';
		string[1] = '0' + number;
		string[2] = '\0';
	}
	else
	{
		while (number != 0)
		{
			digits[--index] = '0' + number % 10;
			number = number / 10;
		}

		for (uint32_t i = index, j = 0; i < 32; i++, j++)
		{
			string[j] = digits[i];
		}
		uint32_t len = 32 - index;
		string[len] = '\0';
	}

	return string;
}

void removeLeadingZeros(char* string)
{
	uint32_t i = 0;
	while(string[i] == '0')
	{
		i++;
	}

	//if all were zeros, leave one zero and terminal character
	if(string[i] == '\0') //end of string
	{
		string[0] = '0';
		string[1] = '\0';
	} else
	{
		memmove(string, string + i, strlen(string + i) + 1);
	}
}

static void Work_Task(void *parameters)
{
	//display on LCD
	digitStr1[0] = tempInputBuffer[0];
	digitStr1[1] = '\0';

	digitStr2[0] = tempInputBuffer[1];
	digitStr2[1] = '\0';

	char *thresholdTempToDisplay = intToString(tempThreshold);

	LCD_DisplayStringAtAddress(0x00, rainfallString);
	LCD_DisplayStringAtAddress(0x50, thresholdTempString1);
	uint32_t baseAddr = 0x50 + strlen(thresholdTempString1);
	LCD_DisplayStringAtAddress(baseAddr, digitStr1);
	LCD_DisplayStringAtAddress(baseAddr + 1, digitStr2);
	LCD_DisplayStringAtAddress(baseAddr + 2, thresholdTempString2);
	LCD_DisplayStringAtAddress(baseAddr + 2 + strlen(thresholdTempString2),
			thresholdTempToDisplay);
	vPortFree(thresholdTempToDisplay);

	while (1)
	{
		//temperature
		temperatureValue = TEMP_GetTemperValue();
		if ((int) temperatureValue != (int) prevTemperautreValue)
		{
			valueChangedFlagVT = 1;
			prevTemperautreValue = temperatureValue;
			if (temperatureValue < 0)
			{
				temperatureToDisplay = 0;
			}
			else if (temperatureValue > 60)
			{
				temperatureToDisplay = 60;
			}
			else
			{
				temperatureToDisplay = (uint32_t) temperatureValue;
			}
		}


		//value changed (temperature), Virtuel Terminal
		if (valueChangedFlagVT)
		{
			valueChangedFlagVT = 0;
			char *temperatureToDisplayString = intToString(
					temperatureToDisplay);

			removeLeadingZeros(temperatureToDisplayString);

			//display on VT
			for (uint32_t i = 0; i < prevLenTemperatureString; i++)
			{
				UART_AsyncTxCharacter('\b');
			}
			UART_AsyncTxString(temperatureString);
			UART_AsyncTxString(temperatureToDisplayString);

			prevLenTemperatureString = strlen(temperatureString)
					+ strlen(temperatureToDisplayString);

			vPortFree(temperatureToDisplayString);
		}


		//The fan speed changes depending on the temperature value (temperatureToDisplay [0,60])
		FanState targetFanState = TURNED_OFF;
		if(temperatureToDisplay >= 0 && temperatureToDisplay < tempThreshold)
		{
			targetFanState = TURNED_OFF; //the fan must be turned off
		} else if (temperatureToDisplay >= tempThreshold && temperatureToDisplay <= 60)
		{

			targetFanState = SLOW; //the fan must rotate at a speed equal to 50% of maximum speed
		}

		uint8_t iterationNum = abs(fanState - targetFanState);
		for(uint8_t i = 0; i < iterationNum; i++)
		{
			if(targetFanState > fanState)
			{
				MOTOR_SpeedIncrease();
			} else
			{
				MOTOR_SpeedDecrease();
			}
		}
		fanState = targetFanState;


		//value changed (rainfall), LCD display
		if (valueChangedFlagLCD)
		{
			valueChangedFlagLCD = 0;
			char *rainfallToDisplayString = intToString(rainfallMmPerHour);
			removeLeadingZeros(rainfallToDisplayString);

			//display on LCD
			LCD_DisplayStringAtAddress(0x00 + strlen(rainfallString),
					rainfallToDisplayString);

			vPortFree(rainfallToDisplayString);
		}

		//value changed (keypad), LCD display
		if (keyValueChanged)
		{
			keyValueChanged = 0;
			char digitStr1[2] =
			{ tempInputBuffer[0], '\0' };
			char digitStr2[2] =
			{ tempInputBuffer[1], '\0' };
			char *thresholdTempToDisplay = intToString(tempThreshold); //new threshold

			LCD_DisplayStringAtAddress(baseAddr, digitStr1);
			LCD_DisplayStringAtAddress(baseAddr + 1, digitStr2);
			LCD_DisplayStringAtAddress(baseAddr + 2, thresholdTempString2);
			LCD_DisplayStringAtAddress(
					baseAddr + 2 + strlen(thresholdTempString2),
					thresholdTempToDisplay);
			vPortFree(thresholdTempToDisplay);
		}

		vTaskDelay(pdMS_TO_TICKS(200));
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	switch (state)
	{
	case WAIT_INITIAL_EDGE:
		if (htim->Instance == TIM2 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
		{
			timestamptStart = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
			state = WAIT_PERIOD_END; //another rising edge
			counterOverflowCounter = 0;
		}
		break;
	case WAIT_PERIOD_END:
		if (htim->Instance == TIM2 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
		{
			timestamptEnd = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
			tickElapsedPeriod = (timestamptEnd
					+ (ARR + 1) * counterOverflowCounter) - timestamptStart;

			elapsedMs = (uint32_t) (tickElapsedPeriod
					* ((float) 1000 / FREQUENCY_CLK_TIM3) + 0.5);
			rainfallMmPerHour = (uint32_t) (((float) 36000 / elapsedMs) + 0.5);

			counterOverflowCounter = 0;
			valueChangedFlagLCD = 1;
			timestamptStart = timestamptEnd;
			state = WAIT_PERIOD_END;
		}
		break;
	}
}

void LED_Callback(TimerHandle_t xTimer)
{
	float currTemp = TEMP_GetTemperValue();
	if (currTemp < tempThreshold) //alarm is off
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); //green on
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET); //red off
	}
	else //alarm is on
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); // green OFF
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9); //toggle red
	}
}

void Work_Init()
{
	UART_Init();
	TEMP_Init();
	LCD_Init();
	KEY_Init();
	MOTOR_Init();

	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3);
	xTaskCreate(Work_Task, "Work_Task", 128, NULL, 2, &WORK_TaskHandle);

	LED_TimerHandle = xTimerCreate("LED_Timer", pdMS_TO_TICKS(500), pdTRUE,
			NULL, LED_Callback);
	xTimerStart(LED_TimerHandle, portMAX_DELAY);
}
