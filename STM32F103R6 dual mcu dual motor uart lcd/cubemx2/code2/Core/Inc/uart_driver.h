/*
 * uart_driver.h
 *
 *  Created on: May 30, 2025
 *      Author: Korisnik
 */

#ifndef CORE_INC_UART_DRIVER_H_
#define CORE_INC_UART_DRIVER_H_

#include "work.h"
#include <stdint.h>

extern void UART_Init();

extern void UART_AsyncTxMotorCommand(MotorCommand motorCommand);

extern MotorCommand UART_BlockRxMotorCommand();


#endif /* CORE_INC_UART_DRIVER_H_ */
