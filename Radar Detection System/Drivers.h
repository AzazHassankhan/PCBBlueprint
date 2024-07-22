/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "project.h"       /* Include project-specific header file */

/* Prototype for UART interrupt service routine */
CY_ISR_PROTO(Rx_Int); 

/* Function to send a counter value via UART */
void UART_func(int counter);

/* Function to start an ADC conversion and return the result */
int ADC_Func();

/* Function to start UART, PWM, and ADC components */
void Start_Components();

/* Function to stop UART, PWM, and ADC components */
void Stop_Components();

/* Function to detect the ADC value */
void LED_Detection(int adc_value);

/* Function to Recive Bytes from Matlab */
unsigned char Byte_Received();

/* Function to set Recive Bytes */
void Set_Byte_Received(unsigned char set_byte);

/* Function to show adc values through Leds */
void LED_Detection(int adcValue);

/* Function to set the Leds */
void LEDs_Set(int a, int b, int c);
/* [] END OF FILE */
