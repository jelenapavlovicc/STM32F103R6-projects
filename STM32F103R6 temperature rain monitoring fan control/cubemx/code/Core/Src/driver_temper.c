/*
 * sensors.c
 *
 *  Created on: Jun 27, 2025
 *      Author: Korisnik
 */

#include "../Inc/driver_temper.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "adc.h"

#define MAX_VOLTAGE		5.0
#define RESOLUTION		4096.0

static float temperatureSensorValue = -1;
static TaskHandle_t TEMP_TaskHandle;
static QueueHandle_t TEMP_MailboxHandle;

//---------------------Temperature task-----------------------------------------

static void TEMP_Task(void* parameters)
{
	while(1)
	{
		HAL_ADC_Start_IT(&hadc1);
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		uint32_t value = HAL_ADC_GetValue(&hadc1); //raw value
		float voltage = ((float) value) * MAX_VOLTAGE / RESOLUTION; //voltage value
		temperatureSensorValue = voltage * 100; //temperature value

		xQueueOverwrite(TEMP_MailboxHandle, &temperatureSensorValue);
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc1.Instance == hadc->Instance)
	{
		BaseType_t woken = pdFALSE;
		vTaskNotifyGiveFromISR(TEMP_TaskHandle, &woken);
		portYIELD_FROM_ISR(woken);
	}
}

//--------------------General---------------------------------------------------

void TEMP_Init()
{
	xTaskCreate(TEMP_Task, "TEMP_Task", 64, NULL, 5, &TEMP_TaskHandle);
	TEMP_MailboxHandle = xQueueCreate(1, sizeof(float));
}

//--------------------Temperature utils-----------------------------------------

float TEMP_GetTemperValue()
{
	float value = -1.0;
	xQueuePeek(TEMP_MailboxHandle, &value, 0);
	return value;
}


