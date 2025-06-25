#include "FreeRTOS.h"
#include "uart_driver.h"
#include "usart.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"
#include <string.h>

static QueueHandle_t UART_TxQueueHandle;
static TaskHandle_t UART_TxTaskHandle;
static SemaphoreHandle_t UART_TxMutexHandle;

//-----------Transmit task------------------------------------------------------

void UART_TxTask(void *parameters)
{
	uint8_t buffer;
	while (1)
	{
		xQueueReceive(UART_TxQueueHandle, &buffer, portMAX_DELAY);
		HAL_UART_Transmit_IT(&huart1, &buffer, sizeof(uint8_t));
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == huart1.Instance)
	{
		BaseType_t woken = pdFALSE;
		vTaskNotifyGiveFromISR(UART_TxTaskHandle, &woken);
		portYIELD_FROM_ISR(woken);
	}
}

//----------General-------------------------------------------------------------

void UART_Init()
{
	xTaskCreate(UART_TxTask, "UART_TxTask", 128, NULL, 4, &UART_TxTaskHandle);
	UART_TxQueueHandle = xQueueCreate(64, sizeof(uint8_t));
	UART_TxMutexHandle = xSemaphoreCreateMutex();
}

//-------Transmit util----------------------------------------------------------

void UART_AsyncTxCharacter(uint8_t buffer)
{
	xSemaphoreTake(UART_TxMutexHandle, portMAX_DELAY);
	xQueueSendToBack(UART_TxQueueHandle, &buffer, portMAX_DELAY);
	xSemaphoreGive(UART_TxMutexHandle);
}

void UART_AsyncTxString(char const *string)
{
	if (string != NULL)
	{
		xSemaphoreTake(UART_TxMutexHandle, portMAX_DELAY);

		for (uint32_t i = 0; i < strlen(string); i++)
		{
			xQueueSendToBack(UART_TxQueueHandle, string + i, portMAX_DELAY);
		}
		xSemaphoreGive(UART_TxMutexHandle);
	}
}
