/*
 * driver_lcd.c
 *
 *  Created on: Aug 25, 2025
 *      Author: Korisnik
 */

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "gpio.h"
#include "driver_lcd.h"

#define LCD_ENABLE_BIT 0x40

static void LCD_Write(LCD_CommandReg reg, LCD_CommandVal val)
{
	GPIOC->ODR = (val & 0x0F) | ((reg & 0x01) << 5);
	GPIOC->ODR |= LCD_ENABLE_BIT;
	GPIOC->ODR &= ~LCD_ENABLE_BIT;
}

static void LCD_CommandInitiate(LCD_CommandReg reg, LCD_CommandVal val)
{
	//4 bit interface
	LCD_Write(reg, val >> 4);
	LCD_Write(reg, val >> 0);
	vTaskDelay(pdMS_TO_TICKS(2));
}

static QueueHandle_t LCD_QueueHandle;
static TaskHandle_t LCD_TaskHandle;

static void LCD_Task(void* parameters)
{
	//initial reset circuit
	vTaskDelay(pdMS_TO_TICKS(20));

	LCD_Write(LCD_INSTRUCTION,
			(LCD_INSTRUCTION_FUNCTION_SET | LCD_FUNCTION_SET_4_BITS_DATA_INTERFACE) >> 4);
	vTaskDelay(pdMS_TO_TICKS(2));

	LCD_CommandInitiate(LCD_INSTRUCTION,
			LCD_INSTRUCTION_FUNCTION_SET |
			LCD_FUNCTION_SET_2_LINES_DISPLAY |
			LCD_FUNCTION_SET_4_BITS_DATA_INTERFACE |
			LCD_FUNCTION_SET_FONT_5x8_DOTS);

	LCD_CommandInitiate(LCD_INSTRUCTION,
			LCD_INSTRUCTION_DISPLAY_CONTROL |
			LCD_DISPLAY_CONTROL_BLINK_OFF |
			LCD_DISPLAY_CONTROL_CURSOR_OFF |
			LCD_DISPLAY_CONTROL_DISPLAY_ON);

	LCD_CommandInitiate(LCD_INSTRUCTION,
			LCD_INSTRUCTION_ENTRY_MODE_SET |
			LCD_ENTRY_MODE_INCREMENT_ADDR |
			LCD_ENTRY_MODE_DISPLAY_SHIFT_OFF);

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
	xTaskCreate(LCD_Task, "LCDTask", 128, NULL, 2, &LCD_TaskHandle);
	LCD_QueueHandle = xQueueCreate(64, sizeof(LCD_Command));
}

void LCD_CommandEnqueue(LCD_CommandReg reg, LCD_CommandVal val)
{
	LCD_Command cmd = {reg, val};
	xQueueSend(LCD_QueueHandle, &cmd, portMAX_DELAY);
}

void LCD_CommandEnqueueFromISR(LCD_CommandReg reg, LCD_CommandVal val,
		BaseType_t *pxHigherPriorityTaskWoken)
{
	LCD_Command cmd = {reg, val};
	xQueueSendFromISR(LCD_QueueHandle, &cmd, pxHigherPriorityTaskWoken);
}
