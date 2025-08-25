/*
 * driver_uart.c
 *
 *  Created on: Aug 24, 2025
 *      Author: Korisnik
 */



#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "driver_uart.h"
#include "usart.h"

#include <string.h>

//----------TRANSMIT------------------------------------------------------------

static TaskHandle_t UART_TxTaskHandle;
static QueueHandle_t UART_TxQueueHandle;
static SemaphoreHandle_t UART_TxMutexHandle;

static void UART_TxTask(void* parameters){
	uint8_t buffer;
	while(1){
		xQueueReceive(UART_TxQueueHandle, &buffer, portMAX_DELAY);
		HAL_UART_Transmit_IT(&huart1, &buffer, sizeof(uint8_t));
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == huart1.Instance){
		BaseType_t woken = pdFALSE;
		vTaskNotifyGiveFromISR(UART_TxTaskHandle, &woken);
		portYIELD_FROM_ISR(woken);
	}
}

//----------RECEIVE-------------------------------------------------------------

static TaskHandle_t UART_RxTaskHandle;
static QueueHandle_t UART_RxQueueHandle;
static SemaphoreHandle_t UART_RxMutexHandle;

static void UART_RxTask(void* parameters){
	uint8_t buffer;
	while(1){
		HAL_UART_Receive_IT(&huart1, &buffer, sizeof(uint8_t));
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		xQueueSendToBack(UART_RxQueueHandle, &buffer, portMAX_DELAY);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == huart1.Instance){
		BaseType_t woken = pdFALSE;
		vTaskNotifyGiveFromISR(UART_RxTaskHandle, &woken);
		portYIELD_FROM_ISR(woken);
	}
}


//---------GENERAL--------------------------------------------------------------

void UART_Init(){
	xTaskCreate(UART_TxTask, "transmitTask", 64, NULL, 4, &UART_TxTaskHandle);
	UART_TxQueueHandle = xQueueCreate(64, sizeof(uint8_t));
	UART_TxMutexHandle = xSemaphoreCreateMutex();

	xTaskCreate(UART_RxTask, "receiveTask", 64, NULL, 20, &UART_RxTaskHandle);
	UART_RxQueueHandle = xQueueCreate(64, sizeof(uint8_t));
	UART_RxMutexHandle = xSemaphoreCreateMutex();
}

//----------Transmit utils------------------------------------------------------

void UART_AsyncTxCharacter(char character){
	xSemaphoreTake(UART_TxMutexHandle, portMAX_DELAY);
	xQueueSendToBack(UART_TxQueueHandle, &character, portMAX_DELAY);
	xSemaphoreGive(UART_TxMutexHandle);
}

void UART_AsyncTxString(char const* string){
	if(string == NULL) return;

	xSemaphoreTake(UART_TxMutexHandle, portMAX_DELAY);

	for(uint32_t i = 0; i < strlen(string); i++){
		xQueueSendToBack(UART_TxQueueHandle, string + i, portMAX_DELAY);
	}

	xSemaphoreGive(UART_TxMutexHandle);
}

void UART_AsyncTxDecimal(uint32_t decimal){
	xSemaphoreTake(UART_TxMutexHandle, portMAX_DELAY);
	char digits[32];
	uint32_t index = 32;
	while(index >= 0 && decimal != 0){
		digits[--index] = decimal % 10 + '0';
		decimal = decimal / 10;
	}

	for(uint32_t i = index; i < 32; i++){
		xQueueSendToBack(UART_TxQueueHandle, digits + i, portMAX_DELAY);
	}
	xSemaphoreGive(UART_TxMutexHandle);
}

//----------Receive utils-------------------------------------------------------

char UART_BlockRxCharacter()
{
	xSemaphoreTake(UART_RxMutexHandle, portMAX_DELAY);
	char character;
	xQueueReceive(UART_RxQueueHandle, &character, portMAX_DELAY);
	xSemaphoreGive(UART_RxMutexHandle);
	return character;
}

char* UART_BlockRxString()
{
	xSemaphoreTake(UART_RxMutexHandle, portMAX_DELAY);
	char* string = pvPortMalloc(64); //64B

	if(string != NULL){
		uint32_t index = 0;
		char character = '\0';

		while(character != '\r' && index < 64)
		{
			xQueueReceive(UART_RxQueueHandle, &character, portMAX_DELAY);
			string[index++] = character;
		}
		string[index--] = '\0';
	}

	xSemaphoreGive(UART_RxMutexHandle);
	return string;
}

uint32_t UART_BlockRxDecimal()
{
	xSemaphoreTake(UART_RxMutexHandle, portMAX_DELAY);
	uint32_t decimal = 0;
	char character = '\0';

	while(character != '\r')
	{
		xQueueReceive(UART_RxQueueHandle, &character, portMAX_DELAY);
		if(character >= '0' && character <= '9')
		{
			decimal = decimal * 10 + (character - '0');
		}
	}

	xSemaphoreGive(UART_RxMutexHandle);
	return decimal;
}

