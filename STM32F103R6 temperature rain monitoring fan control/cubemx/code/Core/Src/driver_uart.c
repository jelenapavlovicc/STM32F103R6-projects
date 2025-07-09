/*
 * uart_driver.c
 *
 *  Created on: Jun 27, 2025
 *      Author: Korisnik
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "usart.h"
#include <string.h>
#include "../Inc/driver_uart.h"

static TaskHandle_t UART_TxTaskHandle;
static QueueHandle_t UART_TxQueueHandle;
static SemaphoreHandle_t UART_TxMutexHandle;

//--------------Transmit task---------------------------------------------------

static void UART_TxTask(void* parameters)
{
	uint8_t buffer;
	while(1)
	{
		xQueueReceive(UART_TxQueueHandle, &buffer, portMAX_DELAY);
		HAL_UART_Transmit_IT(&huart1, &buffer, sizeof(uint8_t));
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart1.Instance == huart->Instance)
	{
		BaseType_t woken = pdFALSE;
		vTaskNotifyGiveFromISR(UART_TxTaskHandle, &woken);
		portYIELD_FROM_ISR(woken);
	}
}

//---------------General--------------------------------------------------------

void UART_Init()
{
	xTaskCreate(UART_TxTask, "UART_Transmit", 128, NULL, 4, &UART_TxTaskHandle);
	UART_TxQueueHandle = xQueueCreate(64, sizeof(uint8_t));
	UART_TxMutexHandle = xSemaphoreCreateMutex();
}

//--------------Transmit utils--------------------------------------------------

void UART_AsyncTxCharacter(char character)
{
	xSemaphoreTake(UART_TxMutexHandle, portMAX_DELAY);
	xQueueSendToBack(UART_TxQueueHandle, &character, portMAX_DELAY);
	xSemaphoreGive(UART_TxMutexHandle);
}

void UART_AsyncTxString(char* string)
{
	if(string != NULL)
	{
		xSemaphoreTake(UART_TxMutexHandle, portMAX_DELAY);
		for(uint32_t i = 0; i < strlen(string); i++)
		{
			xQueueSendToBack(UART_TxQueueHandle, string + i, portMAX_DELAY);
		}
		xSemaphoreGive(UART_TxMutexHandle);
	}
}

void UART_AsyncTxDecimal(uint32_t decimal)
{
	xSemaphoreTake(UART_TxMutexHandle, portMAX_DELAY);

	char digits[32];
	uint32_t index = 32;

	if(decimal == 0)
	{
	    char zero = '0';
	    xQueueSendToBack(UART_TxQueueHandle, &zero, portMAX_DELAY);
	} else
	{
		while(decimal != 0)
		{
			digits[--index] = '0' + decimal % 10;
			decimal = decimal / 10;
		}

		for(uint32_t i = index; i < 32; i++)
		{
			xQueueSendToBack(UART_TxQueueHandle, digits + i, portMAX_DELAY);
		}
	}
	xSemaphoreGive(UART_TxMutexHandle);
}
