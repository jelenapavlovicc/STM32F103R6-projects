/*
 * lcd.c
 *
 *  Created on: May 31, 2025
 *      Author: Korisnik
 */

#include "lcd.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "gpio.h"

#define LCD_ENABLE_BIT 	0x40

static void LCD_Write(LCD_CommandReg reg, LCD_CommandVal val)
{
	GPIOC->ODR = ((reg & 0x01) << 5) | (val & 0x0F) | (0 << 4);

	GPIOC->ODR |= LCD_ENABLE_BIT;
	GPIOC->ODR &= ~LCD_ENABLE_BIT;
}

static void LCD_CommandInitiate(LCD_CommandReg reg, LCD_CommandVal val)
{
	//Data Interface is 4 bits
	LCD_Write(reg, val >> 4);
	LCD_Write(reg, val >> 0);

	vTaskDelay(pdMS_TO_TICKS(2));
}

static TaskHandle_t LCD_TaskHandle;
static QueueHandle_t LCD_QueueHandle;

static void LCD_Task(void* parameters)
{
	//Initializing by Internal Reset Circuit
	vTaskDelay(pdMS_TO_TICKS(20));

	LCD_Write(LCD_INSTRUCTION,
			(LCD_INSTRUCTION_FUNCTION_SET |
			LCD_FUNCTION_SET_4_BITS_INTERFACE) >> 4);
	vTaskDelay(pdMS_TO_TICKS(2));

	LCD_CommandInitiate(LCD_INSTRUCTION,
			LCD_INSTRUCTION_FUNCTION_SET |
			LCD_FUNCTION_SET_2_LINES |
			LCD_FUNCTION_SET_4_BITS_INTERFACE |
			LCD_FUNCTION_SET_5x8_DOTS);

	LCD_CommandInitiate(LCD_INSTRUCTION,
			LCD_INSTRUCTION_DISPLAY_CONTROL |
			LCD_DISPLAY_CONTROL_DISPLAY_ON |
			LCD_DISPLAY_CONTROL_BLINK_OFF |
			LCD_DISPLAY_CONTROL_CURSOR_OFF);

	LCD_CommandInitiate(LCD_INSTRUCTION,
			LCD_INSTRUCTION_ENTRY_MODE_SET |
			LCD_ENTRY_MODE_SET_INCREMENT_ADDR |
			LCD_ENTRY_MODE_SET_DISPLAY_SHIFT_OFF);

	LCD_CommandInitiate(LCD_INSTRUCTION, LCD_INSTRUCTION_RETURN_HOME);

	LCD_Command cmd;
	while(1)
	{
		xQueueReceive(LCD_QueueHandle, &cmd, portMAX_DELAY);
		LCD_CommandInitiate(cmd.reg, cmd.val);
	}
}

void LCD_Init()
{
	xTaskCreate(LCD_Task, "LCD_Task", 128, NULL, 2, &LCD_TaskHandle);
	LCD_QueueHandle = xQueueCreate(64, sizeof(LCD_Command));
}

void LCD_CommandEnqueue(LCD_CommandReg reg, LCD_CommandVal val)
{
	LCD_Command cmd = {reg, val};
	xQueueSend(LCD_QueueHandle, &cmd, portMAX_DELAY);
}

void LCD_CommandEnqueueFromISR(LCD_CommandReg reg, LCD_CommandVal val, BaseType_t *pxHigherPriorityTaskWoken)
{
	LCD_Command cmd = {reg, val};
	xQueueSendFromISR(LCD_QueueHandle, &cmd, pxHigherPriorityTaskWoken);
}
