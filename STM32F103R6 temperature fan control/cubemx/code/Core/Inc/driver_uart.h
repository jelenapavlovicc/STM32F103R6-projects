/*
 * driver_uart.h
 *
 *  Created on: Aug 24, 2025
 *      Author: Korisnik
 */

#ifndef CORE_INC_DRIVER_UART_H_
#define CORE_INC_DRIVER_UART_H_
#include <stdint.h>

extern void UART_Init();

extern void UART_AsyncTxCharacter(char character);
extern void UART_AsyncTxString(char const* string);
extern void UART_AsyncTxDecimal(uint32_t decimal);

extern char UART_BlockRxCharacter();
extern char* UART_BlockRxString();
extern uint32_t UART_BlockRxDecimal();

#endif /* CORE_INC_DRIVER_UART_H_ */
