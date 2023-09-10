/*************************************************************************************************************************
 * [File]: MCU2.c
 * [Date]: 2/9/2023
 * [Objective]: Developing a Smart Fire Fighting System - MCU2.
 * [Drivers]: GPIO - Timer1 - UART - ADC - DC_Motor - LCD
 * [Author]: Youssef Ahmed Zaki
 *************************************************************************************************************************/
#include <avr/io.h>

/* MCAL Layer */
#include "GPIO.h"
#include "TIMER1.h"
#include "UART.h"
#include "ADC.h"

/* HAL Layer */
#include "DC_Motor.h"
#include "LCD.h"

#define MCU1_READY                   0x01
#define MCU2_READY                   0X02

/* MCU2 Cases */
#define RECEIVING_TEMPERATURE        0
#define SENDING_MOTOR_SPEED          1
/********************************************************************************************************
 *                                                                                                      *
 *                                             * Global Variables *                                     *
 *                                                                                                      *
 ********************************************************************************************************/

uint8 MCU2_Sequence = 0;

/********************************************************************************************************
 *                                                                                                      *
 *                                             * MCU2 Main Function *                                   *
 *                                                                                                      *
 ********************************************************************************************************/
int main(void)
{
	uint8 Receive_Temp;
	uint16 Res_Value;
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
	 ********************************************************************************************************/

	UART_Init(&UART_Config);
	ADC_Init(&ADC_Config);
	Timer1_PWM_Mode_Init(&Timer1_Config);
	DcMotor_Init();
	LCD_Init();

	/* let the first three pins in in PORTC as output pins to be connected with LEDs */
	GPIO_SetupPinDirection(PORTD_ID, PIN2_ID, OUTPUT_PIN);
	GPIO_SetupPinDirection(PORTD_ID, PIN3_ID, OUTPUT_PIN);
	GPIO_SetupPinDirection(PORTD_ID, PIN4_ID, OUTPUT_PIN);

	/* Display this message always on the LCD Screen */
	LCD_DisplayString("ADC VALUE = ");

	/* Send to MCU1 that MCU2 is ready to start */
	UART_SendByte(MCU2_READY);

	/********************************************************************************************************
	 *                                                                                                      *
	 *                                           * MCU2 Application Sequence *                              *
	 *                                                                                                      *
	 ********************************************************************************************************/

	while (1)
	{
		/* Move LCD cursor to this position */
		LCD_MoveCursor(1,0);

		/* Check ADC Value and Display it on LCD Screen */
		if(Res_Value >= 1000)
		{
			LCD_IntegerToString(Res_Value);
		}
		else
		{
			LCD_IntegerToString(Res_Value);

			/* Put space in next place when the number is one, two or three digits to prevent noises on LCD */
			LCD_DisplayCharacter(' ');
		}

		switch (MCU2_Sequence)
		{

		/* Receiving temperature from MCU1 and based on it, turn on/off LEDs */
		case RECEIVING_TEMPERATURE:

			/* Synchronization between two MCUs to send/receive data */
			while (UART_ReceiveByte() != MCU1_READY);
			UART_SendByte(MCU2_READY);

			/* Receive the temperature from MCU1 */
			Receive_Temp = UART_ReceiveByte();

			if (Receive_Temp == 200)
			{
				/* Let Res_Value = 256 to slow down the motor to 25% of its speed (1023*25% = 256) */
				Res_Value = 256;

				/* Move to the next step */
				MCU2_Sequence++;
				break;

			}
			else
			{
				/* Select ADC0 to be the ADC selected channel and read the value of the potentiometer */
				Res_Value = ADC_ReadChannel(ADC0);

				if (Receive_Temp < 20)
				{
					/* Turn on only Green LED */
					GPIO_WritePin(PORTD_ID, PIN2_ID, LOGIC_HIGH);
					GPIO_WritePin(PORTD_ID, PIN3_ID, LOGIC_LOW);
					GPIO_WritePin(PORTD_ID, PIN4_ID, LOGIC_LOW);
				}
				else if (Receive_Temp >= 20 && Receive_Temp < 40)
				{
					/* Turn on only Yellow LED */
					GPIO_WritePin(PORTD_ID, PIN2_ID, LOGIC_LOW);
					GPIO_WritePin(PORTD_ID, PIN3_ID, LOGIC_HIGH);
					GPIO_WritePin(PORTD_ID, PIN4_ID, LOGIC_LOW);
				}
				else if (Receive_Temp >= 40)
				{
					/* Turn on only Red LED */
					GPIO_WritePin(PORTD_ID, PIN2_ID, LOGIC_LOW);
					GPIO_WritePin(PORTD_ID, PIN3_ID, LOGIC_LOW);
					GPIO_WritePin(PORTD_ID, PIN4_ID, LOGIC_HIGH);
				}
			}

			/* Move to the next step */
			MCU2_Sequence++;
			break;

		case SENDING_MOTOR_SPEED:

			/* The Motor speed is mainly controlled by the Potentiometer */
			DcMotor_Rotate(CW, Res_Value);

			/* Synchronization between two MCUs to send/receive data */
			UART_SendByte(MCU2_READY);
			while (UART_ReceiveByte() != MCU1_READY);

			/* Check if the ADC value reaches 70% of ADC Max value "1023" (MAX Motor Speed) which is = 716 */
			if (Res_Value >= 716)
			{
				/* Send to MCU1 that Motor speed is reached 70% from its maximum speed  */
				UART_SendByte(70);
			}
			else
			{
				/* Send to MCU1 that Motor Speed is less than 70% from its maximum speed */
				UART_SendByte(0);
			}

			/* return to the previous step */
			MCU2_Sequence--;
			break;
		}
	}
}
