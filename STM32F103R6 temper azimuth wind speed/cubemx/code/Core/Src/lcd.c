#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lcd.h"
#include "gpio.h"
#include <string.h>

#define LCD_ENABLE_BIT 0x04

static TaskHandle_t LCD_TaskHandle;
static QueueHandle_t LCD_QueueHandle;

//direct operation with the LCD microcontroller that has a 4-bit data interface
static void LCD_Write(LCD_CommandReg reg, LCD_CommandVal val)
{
	GPIOC->ODR = ((reg & 0x01) << 0) | (0 << 1) | ((val & 0x0F) << 3);

	GPIOC->ODR |= LCD_ENABLE_BIT;
	GPIOC->ODR &= ~LCD_ENABLE_BIT;
}

static void LCD_CommandInitiate(LCD_CommandReg reg, LCD_CommandVal val)
{
	LCD_Write(reg, val >> 4);
	LCD_Write(reg, val >> 0);

	vTaskDelay(pdMS_TO_TICKS(2));
}

static void LCD_Task(void *parameters)
{
	//Initializing by Internal Reset Circuit
	vTaskDelay(pdMS_TO_TICKS(20));

	//Change to 4-bits Data interface of LCD microcontroller
	LCD_Write(INSTRUCTION_REGISTER,
			(LCD_INSTRUCTION_FUNCTION_SET
					| LCD_FUNCTION_SET_DATA_INTERFACE_4_BITS) >> 4);
	vTaskDelay(pdMS_TO_TICKS(2));

	//Other LCD display microcontroller settings
	LCD_CommandInitiate(INSTRUCTION_REGISTER,
	LCD_INSTRUCTION_FUNCTION_SET |
	LCD_FUNCTION_SET_2_DISPLAY_LINES |
	LCD_FUNCTION_SET_CHARACTER_FONT_5x8_DOTS |
	LCD_FUNCTION_SET_DATA_INTERFACE_4_BITS);

	LCD_CommandInitiate(INSTRUCTION_REGISTER,
	LCD_INSTRUCTION_DISPLAY_CONTROL |
	LCD_DISPLAY_CONTROL_DISPLAY_ON |
	LCD_DISPLAY_CONTROL_CURSOR_OFF |
	LCD_DISPLAY_CONTROL_BLINK_OFF);

	LCD_CommandInitiate(INSTRUCTION_REGISTER,
	LCD_INSTRUCTION_ENTRY_MODE_SET |
	LCD_ENTRY_MODE_SET_INCREMENT_ADDR |
	LCD_ENTRY_MODE_SET_DISPLAY_SHIFT_OFF);

	LCD_CommandInitiate(INSTRUCTION_REGISTER, LCD_INSTRUCTION_RETURN_HOME);

	LCD_Command cmd;
	while (1)
	{
		xQueueReceive(LCD_QueueHandle, &cmd, portMAX_DELAY);
		LCD_CommandInitiate(cmd.reg, cmd.val);
	}
}

void LCD_Init()
{
	xTaskCreate(LCD_Task, "LCD_task", 128, NULL, 2, &LCD_TaskHandle);
	LCD_QueueHandle = xQueueCreate(64, sizeof(LCD_Command));
}

//-----------------UTILITY------------------------------------------------------

void LCD_CommandEnqueue(LCD_CommandReg reg, LCD_CommandVal val)
{
	LCD_Command cmd =
	{ reg, val };
	xQueueSendToBack(LCD_QueueHandle, &cmd, portMAX_DELAY);
}

void LCD_CommandEnqueueFromISR(LCD_CommandReg reg, LCD_CommandVal val,
		BaseType_t *pxHigherPriorityTaskWoken)
{
	LCD_Command cmd =
	{ reg, val };
	xQueueSendToBackFromISR(LCD_QueueHandle, &cmd, pxHigherPriorityTaskWoken);
}

//Write string to LCD memory (DDRAM) starting at the given address
//addr - is an offset within the DDRAM memory
//the function itself takes into account what address it is within the entire memory address space.
void LCD_DisplayStringAtAddress(uint32_t addr, const char *string)
{
	LCD_CommandEnqueue(INSTRUCTION_REGISTER,
			LCD_INSTRUCTION_SET_DDRAM_ADDRESS | (addr & 0x7F));
	for (uint32_t i = 0; i < strlen(string); i++)
	{
		LCD_CommandEnqueue(DATA_REGISTER, string[i]);
	}
}
