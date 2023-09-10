/**************************************************************************************************************
 * File Name: ADC.c
 * Date: 21/7/2023
 * Driver: ATmega32 Analog to Digital Converter (ADC) Driver Source File
 * Author: Youssef Zaki
 *************************************************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Common_Macros.h"
#include "ADC.h"

/***************************************************************************************
 *                                      Global Variables                               *
 ***************************************************************************************/

 volatile uint16 g_ADC_Value = 0;

/***************************************************************************************
 *                                  Interrupt Service Routines                         *
 ***************************************************************************************/
 ISR(ADC_vect)
 {
	 g_ADC_Value = ADC;
 }

/****************************************************************************************
 *                                     Functions Definitions                            *
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
void ADC_Init(const ADC_ConfigType *Config_Ptr)
{
	ADMUX = (ADMUX & 0x3F) | ((Config_Ptr -> Voltage_Ref) << 6);

	SET_BIT(ADCSRA, ADEN);
	SET_BIT(ADCSRA, ADIE);
	ADCSRA = (ADCSRA & 0xF8) | (Config_Ptr -> ADC_Prescalar);

	SET_BIT(ADCSRA, ADATE);
	SFIOR = (SFIOR & 0x1F) | ((Config_Ptr -> Trigger_Source) << 5);
}

/*
 * Description:
 * 1. Configure the ADMUX Register and choose the required ADC Channel.
 * 2. Start Conversion of the ADC.
 * 3. Wait for conversion to be completed and ADIF flag becomes = 1
 * 4. You can clear the flag by writing "One" or automatically by Hardware.
 * 5. return the ADC value.
 */
uint16 ADC_ReadChannel(InputChannel_Select Channel_Select)
{
	/* ADMUX & 1110 0000 (MUX4:0) | (0:7) */
	ADMUX = (ADMUX & 0xE0) | (Channel_Select);
	SET_BIT(ADCSRA,ADSC);

	/* Wait for conversion to complete, ADIF becomes '1' */
	while(BIT_IS_CLEAR(ADCSRA,ADIF));

	/* Clear ADIF by write '1' to it */
	SET_BIT(ADCSRA,ADIF);

	/* Read the digital value from the data register */
	return ADC;
}
