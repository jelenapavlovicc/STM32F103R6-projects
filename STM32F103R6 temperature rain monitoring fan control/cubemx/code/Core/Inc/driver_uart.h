/*
 * uart_driver.h
 *
 *  Created on: Jun 27, 2025
 *      Author: Korisnik
 */

#ifndef CORE_INC_DRIVER_UART_H_
#define CORE_INC_DRIVER_UART_H_

extern void UART_AsyncTxCharacter(char character);

extern void UART_AsyncTxString(char* string);

extern void UART_AsyncTxDecimal(uint32_t decimal);


extern void UART_Init();

#endif /* CORE_INC_DRIVER_UART_H_ */
