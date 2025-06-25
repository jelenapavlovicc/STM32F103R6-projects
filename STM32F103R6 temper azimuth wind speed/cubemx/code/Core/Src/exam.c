#include "exam.h"
#include "lcd.h"
#include "uart_driver.h"
#include "sensors.h"

#include "FreeRTOS.h"
#include "task.h"

#include "timers.h"

#include "gpio.h"
#include <string.h>
#include "string_utils.h"

#define 	TEMPER_MAXLEN			8
#define		AZIMUTH_MAXLEN			7
#define 	AZIMUTH_DECIMAL_DIGITS	1
#define 	ANEMOMETER_MAXLEN		4

static float prevTemperatureSensorValue = -1.0;
static float prevWindvaneSensorValue = -1.0;
static float prevAnemometerWindSpeed = -1.0;

static uint32_t lenStringVt = 0;
static uint32_t prevLenStringVt = 0;

static char temperStringVT[TEMPER_MAXLEN + 1];
static char azimuthStringVT[AZIMUTH_MAXLEN + 1];
static char anemometerStringVT[ANEMOMETER_MAXLEN + 1];

static TimerHandle_t LED_TimerHandle;

uint8_t sensorValueChangedFlag = 0;

void UART_Display()
{
	remove_spaces(azimuthStringVT);
	remove_spaces(anemometerStringVT);
	remove_spaces(temperStringVT);

	if (strlen(azimuthStringVT) == 0)
		strcpy(azimuthStringVT, "?");
	if (strlen(temperStringVT) == 0)
		strcpy(temperStringVT, "?");
	if (strlen(anemometerStringVT) == 0)
		strcpy(anemometerStringVT, "?");

	lenStringVt = strlen(azimuthStringVT) + strlen(temperStringVT)
			+ strlen(anemometerStringVT) + 2 + 1;
	char stringVT[lenStringVt];
	strcpy(stringVT, azimuthStringVT);
	strcat(stringVT, "/");
	strcat(stringVT, anemometerStringVT);
	strcat(stringVT, "/");
	strcat(stringVT, temperStringVT);

	uint32_t eraseLength = prevLenStringVt;

	char stringBackspaceVT[eraseLength + 1];
	for (uint32_t i = 0; i < eraseLength; i++)
	{
		stringBackspaceVT[i] = '\b';
	}
	stringBackspaceVT[eraseLength] = '\0';
	prevLenStringVt = lenStringVt;

	//display on Virtuel Terminal
	UART_AsyncTxString(stringBackspaceVT);
	UART_AsyncTxString(stringVT);
}

void exam_Task(void *parameters)
{
	char *string1 = "Temper: ";
	LCD_DisplayStringAtAddress(0x10, string1); //third line of the LCD display

	char *string2 = "Azimuth: ";
	LCD_DisplayStringAtAddress(0x00, string2); //first line of the LCD display

	char *string3 = "Wind speed: ";
	LCD_DisplayStringAtAddress(0x40, string3); //second line of the LCD display

	char *string4 = "Duty cycle: ";
	LCD_DisplayStringAtAddress(0x50, string4); //fourth line of the LCD display

	while (1)
	{
		//update the value obtained from the temperature sensor
		if ((int) prevTemperatureSensorValue != (int) temperatureSensorValue)
		{
			sensorValueChangedFlag = 1;
			prevTemperatureSensorValue = temperatureSensorValue;
			int tempToDisplay = (int) temperatureSensorValue;
			if (tempToDisplay < 0)
			{
				tempToDisplay = 0;
			}
			if (tempToDisplay > 60)
			{
				tempToDisplay = 60;
			}
			char *temperString = intToStr(tempToDisplay, TEMPER_MAXLEN);
			strncpy(temperStringVT, temperString, TEMPER_MAXLEN + 1);
			LCD_DisplayStringAtAddress((0x10 + strlen(string1)), temperString); //after "Temper: "
			vPortFree(temperString);
		}

		//update the value obtained from the wind vane
		if ((int) prevWindvaneSensorValue != (int) windvaneSensorValue)
		{
			sensorValueChangedFlag = 1;
			prevWindvaneSensorValue = windvaneSensorValue;
			char *azimuthString = floatToStr(windvaneSensorValue,
			AZIMUTH_MAXLEN, AZIMUTH_DECIMAL_DIGITS);
			strncpy(azimuthStringVT, azimuthString, AZIMUTH_MAXLEN + 1);
			LCD_DisplayStringAtAddress((0x00 + strlen(string2)), azimuthString);
			vPortFree(azimuthString);
		}

		//update the value obtained from the anemometer
		if ((int) prevAnemometerWindSpeed != (int) anemometerWindSpeed)
		{
			sensorValueChangedFlag = 1;
			prevAnemometerWindSpeed = anemometerWindSpeed;
			char *anemometerWindSpeedString = floatToStr(anemometerWindSpeed,
			ANEMOMETER_MAXLEN, 0);
			strncpy(anemometerStringVT, anemometerWindSpeedString,
			ANEMOMETER_MAXLEN + 1);
			LCD_DisplayStringAtAddress((0x40 + strlen(string3)),
					anemometerWindSpeedString);
			vPortFree(anemometerWindSpeedString);

			char *dutyCycleString = intToStr(dutyCycle, 4);
			LCD_DisplayStringAtAddress((0x50 + strlen(string4)),
					dutyCycleString);
			vPortFree(dutyCycleString);
		}

		if (sensorValueChangedFlag)
		{
			sensorValueChangedFlag = 0;
			UART_Display();
		}
		vTaskDelay(pdMS_TO_TICKS(200));
	}
}

void LED_Timer(TimerHandle_t xTimer)
{
	if (anemometerWindSpeed < 50)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_13);
	}
}

void Exam_Init()
{
	LCD_Init();
	UART_Init();
	Sensors_Init();
	LED_TimerHandle = xTimerCreate("LEDTimer", pdMS_TO_TICKS(500), pdTRUE, NULL,
			LED_Timer);
	xTimerStart(LED_TimerHandle, portMAX_DELAY);
	xTaskCreate(exam_Task, "exam_Task", 128, NULL, 2, NULL);
}
