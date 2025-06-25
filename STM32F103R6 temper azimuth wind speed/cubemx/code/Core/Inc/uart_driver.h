#ifndef CORE_INC_UART_DRIVER_H_
#define CORE_INC_UART_DRIVER_H_

#include <stdint.h>

extern void UART_AsyncTxCharacter(uint8_t buffer);

extern void UART_AsyncTxString(char const *string);

extern void UART_Init();

#endif /* CORE_INC_UART_DRIVER_H_ */
