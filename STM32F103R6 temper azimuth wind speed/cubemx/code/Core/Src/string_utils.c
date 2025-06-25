#include "string_utils.h"
#include "FreeRTOS.h"

//function that converts int number to a string of width maxLen, right aligned
//When using the function, it is necessary to free the space occupied by
//the string with the vPortFree function
char* intToStr(uint32_t number, uint32_t maxLen)
{
	if (maxLen == 0)
		return NULL;
	char *string = pvPortMalloc(maxLen + 1);
	if (string != NULL)
	{
		//initialization of string with blanks
		for (uint32_t i = 0; i < maxLen; i++)
		{
			string[i] = ' ';
		}

		uint32_t index = maxLen - 1;
		//if number is 0
		if (number == 0)
		{
			string[index] = '0';
		}

		//number other than 0
		while (number != 0 && index < maxLen)
		{
			string[index--] = (number % 10) + '0';
			number /= 10;
		}
		string[maxLen] = '\0';
	}
	return string;
}

//function that converts float number to a string of width maxLen, right aligned
//When using the function, it is necessary to free the space occupied by
//the string with the vPortFree function
char* floatToStr(float number, uint32_t maxLen, uint8_t decimalPlaces)
{
	if (maxLen == 0 || decimalPlaces >= maxLen)
		return NULL;
	char *string = pvPortMalloc(maxLen + 1);

	if (string != NULL)
	{
		//initialization of string with blanks
		for (uint32_t i = 0; i < maxLen; i++)
		{
			string[i] = ' ';
		}
		string[maxLen] = '\0'; //end of string

		//separation of integer and decimal part of float number
		int32_t intPart = (int32_t) number;
		float fraction = number - (float) intPart;
		if (intPart < 0)
			intPart = -intPart;
		if (fraction < 0)
			fraction = -fraction;

		//moving decimal point
		uint32_t index = maxLen - 1;
		for (uint8_t i = 0; i < decimalPlaces; i++)
		{
			fraction = fraction * (float) 10;
		}

		uint32_t fractionPart = (uint32_t) (fraction + (float) 0.5);
		uint32_t integerPart = (uint32_t) intPart;

		//convert fractionPart to string
		for (uint8_t i = 0; i < decimalPlaces; i++)
		{
			if (index < maxLen)
			{
				string[index--] = '0' + (fractionPart % 10);
			}
			fractionPart /= 10;
		}

		//add decimal point '.' if necessary
		if (decimalPlaces > 0 && index < maxLen)
		{
			string[index--] = '.';
		}

		//convert integerPart to string
		if (integerPart == 0 && index < maxLen)
		{
			string[index--] = '0';
		}
		else
		{
			while (integerPart != 0 && index < maxLen)
			{
				string[index--] = '0' + integerPart % 10;
				integerPart /= 10;
			}
		}

		//add minus '-' if necessary
		if (number < 0)
		{
			string[index--] = '-';
		}
	}
	return string;
}

//remove spaces from string
void remove_spaces(char *string)
{
	char *readPtr = string;
	char *writePtr = string;

	while (*readPtr != '\0')
	{
		if (*readPtr != ' ')
		{
			*writePtr = *readPtr;
			writePtr++;
		}
		readPtr++;
	}
	*writePtr = '\0';  //end of string
}
