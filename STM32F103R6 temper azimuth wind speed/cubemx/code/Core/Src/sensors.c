#include "sensors.h"
#include "FreeRTOS.h"
#include "task.h"
#include "gpio.h"
#include "adc.h"
#include "tim.h"

#define MAX_VOLTAGE 			5.0
#define RESOLUTION	 			4096
#define TOLERANCE 				20
#define NUMBER_OF_MAPPINGS  	sizeof(mappingTable) / sizeof(mappingTable[0])
#define FREQUENCY_TIM_CLK		8000000
#define ARR						65535

typedef struct
{
	int adcValue;
	float angelInDegrees;
} WindvaneReading;

typedef enum
{
	WAIT_INITIAL_RISING_EDGE = 0, WAIT_FALLING_EDGE, WAIT_PERIOD_END
} PeriodStateMachine;

static WindvaneReading mappingTable[] =
{
{ 952, 0.0 },
{ 2471, 22.5 },
{ 2250, 45.0 },
{ 3760, 67.5 },
{ 3723, 90.0 },
{ 3831, 112.5 },
{ 3356, 135.0 },
{ 3589, 157.5 },
{ 2946, 180.0 },
{ 3116, 202.5 },
{ 1575, 225.0 },
{ 1698, 247.5 },
{ 315, 270.0 },
{ 786, 292.5 },
{ 547, 315.0 },
{ 1285, 337.5 } };

static uint8_t temperatureMeasurementFlag = 0;
float temperatureSensorValue = 0.0;
float windvaneSensorValue = 0.0;
float anemometerWindSpeed = 0.0;

static uint16_t volatile timestampStart = 0;
static uint16_t volatile timestampFall = 0;
static uint16_t volatile timestampEnd = 0;
static uint32_t volatile ticksElapsedPeriod = 0;
static uint32_t volatile ticksActiveElapsedPeriod = 0;
static float volatile frequency = 0.0;
uint32_t volatile dutyCycle = 0;

uint32_t volatile counterOverflowCounter = 0;
static volatile TickType_t lastEdgeTick = 0;
static PeriodStateMachine state = WAIT_INITIAL_RISING_EDGE;

static TaskHandle_t Sensors_TaskHandle;

static void Sensors_Task(void *parameters)
{
	while (1)
	{

		//Temperature measurement
		temperatureMeasurementFlag = 1;
		//CBA = 000 (74HC4051) - select X0
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		//start ADC and waiting for result
		HAL_ADC_Start_IT(&hadc1);
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		temperatureMeasurementFlag = 0;

		//Windvane measurement
		//CBA = 001 (74HC4051) - select X1
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_ADC_Start_IT(&hadc1);
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		//if the anemometer signal is missing for more than 200 ms
		TickType_t currentTick = xTaskGetTickCount();
		if ((currentTick - lastEdgeTick) > pdMS_TO_TICKS(200))
		{
			Anemometer_TimeoutReset();
		}

		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

//Converts ADC values ​​(values ​​obtained from an analog-to-digital converter)
//into wind direction angles expressed in degrees
//Returns a float, which is the angle (in degrees) corresponding to that adcValue
float windVaneAdcToDegrees(int adcValue)
{
	for (uint32_t i = 0; i < NUMBER_OF_MAPPINGS; i++)
	{
		if ((mappingTable[i].adcValue - TOLERANCE <= adcValue)
				&& (mappingTable[i].adcValue + TOLERANCE >= adcValue))
		{
			return mappingTable[i].angelInDegrees;
		}
	}
	return -1.0;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if (hadc->Instance == hadc1.Instance)
	{
		uint32_t value = HAL_ADC_GetValue(&hadc1);

		if (temperatureMeasurementFlag)
		{
			float voltage = ((float) value * MAX_VOLTAGE) / RESOLUTION;
			temperatureSensorValue = voltage * 100;
		}
		else
		{
			windvaneSensorValue = windVaneAdcToDegrees(value);
		}
		xTaskNotifyGive(Sensors_TaskHandle);
	}
}

//code for calculating the frequency and duty cycle for anemometer signal-------
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	switch (state)
	{
	case WAIT_INITIAL_RISING_EDGE:
		if (htim->Instance == htim1.Instance
				&& htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			timestampStart = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			lastEdgeTick = xTaskGetTickCountFromISR();

			counterOverflowCounter = 0;
			state = WAIT_FALLING_EDGE;
		}
		break;

	case WAIT_FALLING_EDGE:
		if (htim->Instance == htim1.Instance
				&& htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			timestampFall = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			ticksActiveElapsedPeriod = (timestampFall
					+ counterOverflowCounter * (ARR + 1)) - timestampStart;
			lastEdgeTick = xTaskGetTickCountFromISR();

			state = WAIT_PERIOD_END;
		}
		break;

	case WAIT_PERIOD_END:
		if (htim->Instance == htim1.Instance
				&& htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			timestampEnd = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			ticksElapsedPeriod = (timestampEnd
					+ counterOverflowCounter * (ARR + 1)) - timestampStart;
			lastEdgeTick = xTaskGetTickCountFromISR();

			frequency = ((float) FREQUENCY_TIM_CLK) / ticksElapsedPeriod;
			dutyCycle = ticksActiveElapsedPeriod * 100 / ticksElapsedPeriod;

			anemometerWindSpeed = frequency * 2.4;

			timestampStart = timestampEnd;
			state = WAIT_FALLING_EDGE;
			counterOverflowCounter = 0;
		}
		break;
	}
}

void Anemometer_TimeoutReset()
{
	anemometerWindSpeed = 0.0;
	dutyCycle = 0;
	frequency = 0.0;
	timestampStart = 0;
	timestampFall = 0;
	timestampEnd = 0;
	ticksActiveElapsedPeriod = 0;
	ticksElapsedPeriod = 0;
	counterOverflowCounter = 0;
	state = WAIT_INITIAL_RISING_EDGE;
}

void Sensors_Init()
{
	htim1.Instance->SR &= ~TIM_SR_UIF;
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_2);
	xTaskCreate(Sensors_Task, "Sensors_Task", 128, NULL, 5,
			&Sensors_TaskHandle);
}
