/*
 * driver_temp.c
 *
 *  Created on: Aug 24, 2025
 *      Author: Korisnik
 */


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "driver_temp.h"
#include "adc.h"


#define MAX_VOLTAGE		5.0
#define ADC_RESOLUTION	4096.0

static TaskHandle_t TEMP_TaskHandle;
static QueueHandle_t TEMP_MailboxHandle;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == hadc1.Instance)
	{
		BaseType_t woken = pdFALSE;
		vTaskNotifyGiveFromISR(TEMP_TaskHandle, &woken);
		portYIELD_FROM_ISR(woken);
	}
}

static void TEMP_Task(void* parameters)
{
	while(1)
	{
		HAL_ADC_Start_IT(&hadc1);
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		float value = HAL_ADC_GetValue(&hadc1); //raw
		value *= MAX_VOLTAGE / ADC_RESOLUTION; // voltage value
		value *= 100; // into temperature

		xQueueOverwrite(TEMP_MailboxHandle, &value);
		vTaskDelay(pdMS_TO_TICKS(200));
	}
}

float TEMP_GetTemperatureValue()
{
	float result = 0.0;
	xQueuePeek(TEMP_MailboxHandle, &result, portMAX_DELAY);
	return result;
}

void TEMP_Init()
{
	xTaskCreate(TEMP_Task, "tempTask", 64, NULL, 2, &TEMP_TaskHandle);
	TEMP_MailboxHandle = xQueueCreate(1, sizeof(float));
}
