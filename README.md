# Emergency_System
Summary: Developing an emergency system to turn on the fan based on motor speed and to turn on LEDs based on temperature. The Project is based on AVR ATMega32 Microcontrollers using MCAL Drivers: {GPIO - TIMER1 - UART - ADC} HAL Drivers: { LCD - LM35 Temperature Sensor - DC Motor }
Description: 
There are two ATMEGA32 MCUs communicate with each other through UART Communication protocol. The MCU1 is interfaced with (LCD - LM35 - DC Motor), but MCU2 is interfaced with (LCD - DC Motor - Three LEDs). 
The MCU1 should read the temperature through one of ADC peripheral channels, then processing the data and display the temperature on LCD screen and sending it to MCU2. 
The MCU2 should receive the temperature from MCU1 and turn on three LEDs based on the temperature:
if the tempertaure < 20 -> Turn on Green LED, if the temperature >= 20 && temperature < 40 -> Turn on the yellow LED, if the temperature >= 40 -> Turn on red LED. 
The MCU2 Dc Motor is operated based on potentiometer to control its speed and sending continually the speed to MCU1 to check it if it is greater than 70% of motor max speed, so the fan will turn on, otherwise it turns off. 
Finally if emergency button of MCU1 is down, so MCU2 should slow down the motor speed to 25% duty cycle and hence the Fan in MCU1 will be turned off. 
