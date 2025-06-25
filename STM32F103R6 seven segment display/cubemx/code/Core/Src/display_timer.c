
#include "display_timer.h"
#include "gpio.h"
#include "tim.h"

TIM_HandleTypeDef* tim_handler = &htim1;

extern uint32_t overflow_counter_counter;
extern uint8_t volatile display_frequency;

uint8_t current_digit = 0;
uint8_t digits[4] = {0, 0, 0, 0};

uint32_t update_event_counter = 0;
uint32_t seconds = 0;
uint32_t minutes = 0;

uint8_t seven_seg_digits [] = {
		0x81, 0xCF, 0x92, 0x86, 0xCC, 0xA4, 0xA0, 0x8F, 0x80, 0x84
};

void display_timer_init()
{
	HAL_TIM_Base_Start_IT(tim_handler);
	HAL_TIM_IC_Start_IT(tim_handler, TIM_CHANNEL_1);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == tim_handler->Instance)
	{
		overflow_counter_counter++;
		if(++update_event_counter == 100)
		{
			update_event_counter = 0;
			if(++seconds == 60)
			{
				seconds = 0;
				if(++minutes == 60)
				{
					minutes = 0;
				}
			}

			if(display_frequency == 0) //show time
			{
				digits[0] = minutes / 10; //highest digit
				digits[1] = minutes % 10;
				digits[2] = seconds / 10;
				digits[3] = seconds % 10;
			}
		}

		//clear previous combination for A, B, C, D, E, F, G and 1, 2, 3, 4
		GPIOC->ODR &= ~0xFFF; // 0 for A, B, C, D, E, F and 0 for 1, 2, 3, 4
		//GPIOC->ODR &= ~0xF00; //0 for 1, 2, 3, 4

		//next digits
		current_digit = (current_digit + 1) % 4;
		GPIOC->ODR |= seven_seg_digits[digits[current_digit]]; //one digit
		GPIOC->ODR |= 0x1 << (current_digit + 8); //one anode
	}
}
