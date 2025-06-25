/*
 * work.c
 *
 *  Created on: May 28, 2025
 *      Author: Korisnik
 */

#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "uart_driver.h"
#include "work.h"
#include <string.h>

//in this array the speed of each electric motor is stored, initial speed is 0
uint8_t velocity_of_motors[2] = {0, 0};

static void workTask(void* parameters)
{
	while(1)
	{
		UART_AsyncTxString(VTE, "\rEnter a two-character command."
				"\rSelect the motor: 1 (Motor 1) or 2 (Motor 2)."
				"\rSpecify the action: i (accelerate) or d (decelerate)."
				"\re.q. 1i\r");
		char* input = UART_BlockRxString(VTE);

		if(input != NULL)
		{
			if(strlen(input) == 2 && (input[0] == '1' || input[0] == '2')
					&& (input[1] == 'd' || input[1] == 'i'))
			{
				uint8_t motor = input[0] - '0'; //index of motor as decimal number, 1 or 2
				char command = input[1]; //i or d as char
				uint8_t index = motor - 1; //index of motor

				switch (command)
				{
					case 'd': //decrease
						if(velocity_of_motors[index] > 0)
						{
							velocity_of_motors[index]--;
						}
						break;
					case 'i': //increase
						if(velocity_of_motors[index] < 16)
						{
							velocity_of_motors[index]++;
						}
						break;
				}
				UART_AsyncTxString(VTE, input);

				MotorCommand motorCommand = {.motor = motor, .velocity = velocity_of_motors[index]};
				UART_AsyncTxMotorCommand(MCU, motorCommand);
			} else
			{
				UART_AsyncTxString(VTE, "Invalid input. Enter the correct command.\r");
			}
			vPortFree(input);
		} else
		{
			UART_AsyncTxString(VTE, "\rEmpty input.\r");
		}
	}
}

void workInit()
{
	UART_Init();
	xTaskCreate(workTask, "workTask", 128, NULL, 5, NULL);
}
