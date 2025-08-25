/*
 * work.h
 *
 *  Created on: Aug 24, 2025
 *      Author: Korisnik
 */

#ifndef CORE_INC_WORK_H_
#define CORE_INC_WORK_H_

typedef enum
{
	TURNED_OFF, SLOW, FAST
} FanState;

extern void work_Init();

#endif /* CORE_INC_WORK_H_ */
