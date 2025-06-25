#ifndef CORE_INC_STRING_UTILS_H_
#define CORE_INC_STRING_UTILS_H_

#include <string.h>
#include <stdint.h>

//function that converts int number to a string of width maxLen, right aligned
//When using the function, it is necessary to free the space occupied by
//the string with the vPortFree function
extern char* intToStr(uint32_t number, uint32_t maxLen);

//function that converts float number to a string of width maxLen, right aligned
//When using the function, it is necessary to free the space occupied by
//the string with the vPortFree function
extern char* floatToStr(float number, uint32_t maxLen, uint8_t decimalPlaces);

//remove spaces from string
extern void remove_spaces(char *string);

#endif /* CORE_INC_STRING_UTILS_H_ */
