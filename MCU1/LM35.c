/*****************************************************************************************************************
 * File Name: LM35.c
 * Date: 6/8/2023
 * Driver: LM35 Temperature Sensor Driver Source File
 * Author: Youssef Zaki
 ****************************************************************************************************************/
#include "LM35.h"
#include "ADC.h"

/****************************************************************************************
 *                                     Functions Definitions                            *
 ****************************************************************************************/

/*
 * Description:
 * Calculation of the Temperature Sensor, then return the temperature.
 */
uint8 LM35_GetTemperature(void)
{
	uint8 Temperature = 0;

	uint16 Digital_Value = 0;

	Digital_Value = ADC_ReadChannel(LM35_SENSOR_READ_CHANNEL);

	Temperature = ( ((uint32)Digital_Value  * MAX_VOLTAGE_REFERENCE * MAX_LM35_TEMPERATURE) / ( MAX_VOLTAGE_SENSOR * ADC_MAX_DIGITAL_VALUE) );

	return Temperature;
}
