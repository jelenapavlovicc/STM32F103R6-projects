#ifndef CORE_INC_SENSORS_H_
#define CORE_INC_SENSORS_H_

#include <stdint.h>

extern float temperatureSensorValue;
extern float windvaneSensorValue;
extern float anemometerWindSpeed;

extern uint32_t volatile counterOverflowCounter;

extern uint32_t volatile dutyCycle;

extern void Anemometer_TimeoutReset();

extern void Sensors_Init();

#endif /* CORE_INC_SENSORS_H_ */
