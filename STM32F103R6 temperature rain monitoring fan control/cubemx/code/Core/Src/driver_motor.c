/*
 * driver_motor.c
 *
 *  Created on: Jul 9, 2025
 *      Author: Korisnik
 */


#include "driver_motor.h"
#include "tim.h"

#define ARR		9
#define STEP 	2
#define INCREMENT ((ARR + 1) / STEP)

static uint32_t volatile compareRegisterValue = 0; //the initial motor speed is 0

void MOTOR_SpeedIncrease()
{
	if(compareRegisterValue < ARR + 1)
	{
		compareRegisterValue += INCREMENT;
	}
	htim3.Instance->CCR1 = compareRegisterValue;
}

void MOTOR_SpeedDecrease()
{
	if(compareRegisterValue > 0)
	{
		compareRegisterValue -= INCREMENT;
	}
	htim3.Instance->CCR1 = compareRegisterValue;
}

void MOTOR_Init()
{
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}
