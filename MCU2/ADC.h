/**************************************************************************************************************
 * File Name: ADC.h
 * Date: 21/7/2023
 * Driver: ATmega32 Analog to Digital Converter (ADC) Header File
 * Author: Youssef Zaki
 *************************************************************************************************************/
#include "Standard_Types.h"

#ifndef ADC_H_
#define ADC_H_

/******************************************************************************************
 *                                    Macros Definitions                                  *
 ******************************************************************************************/
#define ADC_VOLTAGE_REF          5
#define ADC_MAX_VALUE            1023

/*******************************************************************************************
 *                                    External Variables                                   *
 *******************************************************************************************/

/* Extern Public global variable to be used by other modules */
extern volatile uint16 g_adcResult;

/*******************************************************************************************
 *                                      Types Declaration                                  *
 *******************************************************************************************/
typedef enum
{
	ADC_AREF, ADC_AVCC, ADC_Reversed, ADC_Internal_VREF
}VoltageReference_Select;

typedef enum
{
	ADC0, ADC1, ADC2, ADC3, ADC4, ADC5, ADC6, ADC7
}InputChannel_Select;

typedef enum
{
	CLK_2A, CLK2B, CLK_4, CLK_8, CLK_16, CLK_32, CLK_64, CLK_128
}ADC_ClockSelect;

typedef enum
{
	Free_Running, Analog_Comparator, EXT_INT_Req0, TIMER0_COMP, TIMER0_OVF, TIMER1_COMPB, TIMER1_OVF, TIMER1_CAPT
}ADC_AutoTriggerSource;

typedef struct
{
	VoltageReference_Select Voltage_Ref;
	ADC_ClockSelect ADC_Prescalar;
	ADC_AutoTriggerSource Trigger_Source;
}ADC_ConfigType;

/****************************************************************************************
 *                                      Functions Prototypes                            *
 ****************************************************************************************/

/*
 * Description:
 * 1. Choose the voltage reference by doing configuration on bits 6 & 7 in ADMUX Register.
 * 2. Activate the ADEN bit in ADCSRA Register to enable the ADC.
 * 3. Activate the ADIE bit in ADCSRA Register for ADC Interrupt Enable.
 * 4. Choose the required pre-scalar to make ADC operating frequency within a range (50KHz:200KHz).
 * 5. Enable the ADATE bit in ADCSRA Register to activate ADC Auto Trigger Source.
 * 6. By the Configuration of SFIOR Register, choose the ADC Auto Trigger Source from bits ADTS2:0.
 */
void ADC_Init(const ADC_ConfigType *Config_Ptr);

/*
 * Description:
 * 1. Configure the ADMUX Register and choose the required ADC Channel.
 * 2. Start Conversion of the ADC.
 */
uint16 ADC_ReadChannel(InputChannel_Select Channel_Select);


#endif /* ADC_H_ */
