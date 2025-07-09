/*
 * driver_motor.h
 *
 *  Created on: Jul 9, 2025
 *      Author: Korisnik
 */

#ifndef CORE_INC_DRIVER_MOTOR_H_
#define CORE_INC_DRIVER_MOTOR_H_

extern void MOTOR_SpeedIncrease();
extern void MOTOR_SpeedDecrease();

extern void MOTOR_Init();

typedef enum
{
	TURNED_OFF, SLOW, FAST
} FanState;

#endif /* CORE_INC_DRIVER_MOTOR_H_ */
