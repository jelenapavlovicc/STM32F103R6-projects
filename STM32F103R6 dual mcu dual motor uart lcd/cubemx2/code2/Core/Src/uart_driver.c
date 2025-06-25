/*
 * uart_driver.c
 *
 *  Created on: May 30, 2025
 *      Author: Korisnik
 */


#include "uart_driver.h"
#include "usart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <string.h>

static UART_HandleTypeDef* phuart =  &huart1;

//---------------------------TRANSMIT-------------------------------------------

static TaskHandle_t UART_TxTaskHandle;
static QueueHandle_t UART_TxQueueHandle;
static SemaphoreHandle_t UART_TxMutexHandle;

static void UART_TxTask(void* parameters)
{
	uint8_t buffer;
	while(1)
	{
		xQueueReceive(UART_TxQueueHandle, &buffer, portMAX_DELAY);
		HAL_UART_Transmit_IT(phuart, &buffer, sizeof(uint8_t));
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == phuart->Instance)
	{
		BaseType_t woken = pdFALSE;
		vTaskNotifyGiveFromISR(UART_TxTaskHandle, &woken);
		portYIELD_FROM_ISR(woken);
	}
}

//---------------------------RECEIVE--------------------------------------------

static TaskHandle_t UART_RxTaskHandle;
static QueueHandle_t UART_RxQueueHandle;
static SemaphoreHandle_t UART_RxMutexHandle;

static void UART_RxTask(void* parameters)
{
	uint8_t buffer;
	while(1)
	{
		HAL_UART_Receive_IT(phuart, &buffer, sizeof(uint8_t));
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		xQueueSendToBack(UART_RxQueueHandle, &buffer, portMAX_DELAY);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == phuart->Instance)
	{
		BaseType_t woken = pdFALSE;
		vTaskNotifyGiveFromISR(UART_RxTaskHandle, &woken);
		portYIELD_FROM_ISR(woken);
	}
}

//---------------------------GENERAL--------------------------------------------

void UART_Init()
{
	xTaskCreate(UART_TxTask, "transmitTask", 128, NULL, 4, &UART_TxTaskHandle);
	UART_TxQueueHandle = xQueueCreate(64, sizeof(uint8_t));
	UART_TxMutexHandle = xSemaphoreCreateMutex();

	xTaskCreate(UART_RxTask, "receiveTask", 128, NULL, 20, &UART_RxTaskHandle);
	UART_RxQueueHandle = xQueueCreate(64, sizeof(uint8_t));
	UART_RxMutexHandle = xSemaphoreCreateMutex();
}

//---------------------------TRANSMIT UTIL--------------------------------------

void UART_AsyncTxMotorCommand(MotorCommand motorCommand)
{
	xSemaphoreTake(UART_TxMutexHandle, portMAX_DELAY);

	xQueueSendToBack(UART_TxQueueHandle, &motorCommand.motor, portMAX_DELAY);
	xQueueSendToBack(UART_TxQueueHandle, &motorCommand.velocity, portMAX_DELAY);

	xSemaphoreGive(UART_TxMutexHandle);
}

//---------------------------RECEIVE UTIL---------------------------------------

MotorCommand UART_BlockRxMotorCommand()
{
	xSemaphoreTake(UART_RxMutexHandle, portMAX_DELAY);

	MotorCommand motorCommand;

	xQueueReceive(UART_RxQueueHandle, &motorCommand.motor, portMAX_DELAY);
	xQueueReceive(UART_RxQueueHandle, &motorCommand.velocity, portMAX_DELAY);

	xSemaphoreGive(UART_RxMutexHandle);
	return motorCommand;
}
