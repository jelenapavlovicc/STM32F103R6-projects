/*
 * driver_keypad.h
 *
 *  Created on: Jul 7, 2025
 *      Author: Korisnik
 */

#ifndef CORE_INC_DRIVER_KEYPAD_H_
#define CORE_INC_DRIVER_KEYPAD_H_

extern char tempInputBuffer[3];
extern volatile uint32_t tempThreshold;
extern uint8_t keyValueChanged;
extern uint8_t thresholdUpdatePending;

extern void KEY_Init();

#endif /* CORE_INC_DRIVER_KEYPAD_H_ */

