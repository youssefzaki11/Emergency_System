/*************************************************************************************************************************
 * [File]: MCU1.c
 * [Date]: 2/9/2023
 * [Objective]: Developing a Smart Fire Fighting System - MCU1.
 * [Drivers]: GPIO - Timer1 - ADC - UART - DC_Motor - LM35 Temperature Sensor - LCD
 * [Author]: Youssef Ahmed Zaki
 *************************************************************************************************************************/
#include <avr/io.h>

/* MCAL Layer */
#include "GPIO.h"
#include "TIMER1.h"
#include "ADC.h"
#include "UART.h"

/* HAL Layer */
#include "DC_Motor.h"
#include "LM35.h"
#include "LCD.h"

#define MCU1_READY                   0x01
#define MCU2_READY                   0X02

/* MCU1 Cases */
#define SENDING_TEMPERATURE          0
#define RECEIVING_MOTOR_SPEED        1
/********************************************************************************************************
 *                                                                                                      *
 *                                             * Global Variables *                                     *
 *                                                                                                      *
 ********************************************************************************************************/

uint8 MCU1_Sequence = 0;

/********************************************************************************************************
 *                                                                                                      *
 *                                             * MCU1 Main Function *                                   *
 *                                                                                                      *
 ********************************************************************************************************/
int main(void)
{
	uint8 Temp = 0;
	uint8 Receive_Speed;
	/********************************************************************************************************
	 *                                                                                                      *
	 *                                         * Drivers Configurations *                                   *
	 *                                                                                                      *
	 ********************************************************************************************************/

	/*
	 * ADC Configuration:
	 * Voltage Reference = AREF
	 * Pre-scaler = F_CPU/8
	 * Free Running Mode
	 */
	ADC_ConfigType ADC_Config = {ADC_AREF, CLK_8, Free_Running};

	/*
	 * Timer1 PWM Mode Configuration:
	 * 1. TCNT1 = 0 -> Starting Value of Timer is Zero.
	 * 2. OCR1A = 0 -> It is based on the Duty_Cycle of the PWM Signal.
	 * 3. Pre-scalar = F_CPU/8 -> To control DC motor using 500Hz PWM Signal.
	 * 4. Timer1 Mode -> Fast PWM Mode - 8-bit.
	 */
	Timer1_ConfigType Timer1_Config = {0, 0, TIMER1_Prescaler_8, TIMER1_Fast_Pwm_10_Bit_7};

	/*
	 * UART Configuration:
	 * 1. UART Mode -> Asynchronous Mode.
	 * 2. Data Transmission Speed Mode -> Double Speed Mode.
	 * 3. Parity Bit State -> Disabled.
	 * 4. Data Size in Bits -> Eight bits.
	 * 5. Baud Rate -> 9600.
	 */
	UART_ConfigType UART_Config = {Asynchronous, Double_Speed, Disabled, 0, Eight_Bit_3, 9600};

	/********************************************************************************************************
	 *                                                                                                      *
	 *                                           * Drivers Initialization *                                 *
	 *                                                                                                      *
	 ********************************************************************************************************/\

	 ADC_Init(&ADC_Config);
	 UART_Init(&UART_Config);

	 DcMotor_Init();
	 LCD_Init();

	 /* let the pin 2 in in PORTD as input pin to be connected with push button */
	 GPIO_SetupPinDirection(PORTD_ID, PIN2_ID, INPUT_PIN);

	 /* let the pin 2 in in PORTA (ADC channel 0) as input pin to be connected with LM35 Temperature Sensor */
	 GPIO_SetupPinDirection(PORTA_ID, PIN0_ID, INPUT_PIN);

	 /* Display this message always on the LCD Screen */
	 LCD_DisplayString("Temp =    C");

	 /* Wait until MCU2 is ready */
	 while (UART_ReceiveByte() != MCU2_READY);

	 /********************************************************************************************************
	  *                                                                                                      *
	  *                                           * MCU1 Application Sequence *                              *
	  *                                                                                                      *
	  ********************************************************************************************************/

	 while (1)
	 {
		 /* Read the temperature from the sensor */
		 Temp = LM35_GetTemperature();

		 /* Move the cursor to write the read temperature */
		 LCD_MoveCursor(0,7);

		 if (Temp >= 100)
		 {
			 /* Display the temperature on LCD */
			 LCD_IntegerToString(Temp);
		 }
		 else
		 {
			 /* Display the temperature on LCD */
			 LCD_IntegerToString(Temp);

			 /* Put space in next place when the number is one or two digits to prevent noises on LCD */
			 LCD_DisplayCharacter(' ');
		 }

		 switch(MCU1_Sequence)
		 {
		 case SENDING_TEMPERATURE:

			 /* Synchronization between two MCUs to send/receive data */
			 UART_SendByte(MCU1_READY);
			 while (UART_ReceiveByte() != MCU2_READY);

			 if (GPIO_ReadPin(PORTD_ID, PIN2_ID) == LOGIC_HIGH)
			 {
				 /* Send this serial code to MCU2 to start slowing down the motor */
				 UART_SendByte(200);
			 }
			 else
			 {
				 /* Send the temperature to MCU2 */
				 UART_SendByte(Temp);
			 }

			 /* Move to the next step */
			 MCU1_Sequence++;
			 break;

		 case RECEIVING_MOTOR_SPEED:

			 /* Synchronization between two MCUs to send/receive data */
			 while (UART_ReceiveByte() != MCU2_READY);
			 UART_SendByte(MCU1_READY);

			 /* Receive the DC Motor Speed from MCU2 through UART */
			 Receive_Speed = UART_ReceiveByte();

			 if (Receive_Speed == 70)
			 {
				 /* Initialize the Timer1 in PWM mode */
				 Timer1_PWM_Mode_Init(&Timer1_Config);

				 /* Start the fan */
				 DcMotor_Rotate(CW, 100);
			 }
			 else
			 {
				 /* Stop the fan */
				 DcMotor_Rotate(STOP, 0);

				 /* Stop the timer */
				 Timer1_DeInit();
			 }

			 /* return to the previous step */
			 MCU1_Sequence--;
			 break;
		 }
	 }
}
