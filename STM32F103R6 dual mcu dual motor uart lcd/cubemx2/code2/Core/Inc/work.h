/*
 * work.h
 *
 *  Created on: May 30, 2025
 *      Author: Korisnik
 */

#ifndef CORE_INC_WORK_H_
#define CORE_INC_WORK_H_

#include <stdint.h>

extern void workInit();

typedef struct
{
	uint8_t motor;
	uint8_t velocity;
} MotorCommand;


#endif /* CORE_INC_WORK_H_ */
