/*****************************************************************************************************************
 * File Name: LM35.h
 * Date: 6/8/2023
 * Driver: LM35 Temperature Sensor Driver Header File
 * Author: Youssef Zaki
 ****************************************************************************************************************/
#include "Standard_Types.h"

#ifndef LM35_H_
#define LM35_H_

/******************************************************************************************
 *                                    Macros Definitions                                  *
 ******************************************************************************************/
#define MAX_LM35_TEMPERATURE                 150
#define MIN_LM35_TEMPERATURE                -55

#define MAX_VOLTAGE_REFERENCE                5
#define MAX_VOLTAGE_SENSOR                   1.5
#define ADC_MAX_DIGITAL_VALUE                1023

#define LM35_SENSOR_READ_CHANNEL             2

/******************************************************************************************
 *                                    Functions Prototypes                                *
 ******************************************************************************************/

/*
 * Description:
 * Calculation of the Temperature Sensor, then return the temperature.
 */
uint8 LM35_GetTemperature(void);

#endif /* LM35_H_ */
