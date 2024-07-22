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
#include "stdio.h"         /* Include standard input/output library */

/* Variable to store received byte from UART */
static unsigned char Received_Byte = 0; 

CY_ISR_PROTO(Button_Int);
CY_ISR_PROTO(Rx_Int);

/* Function to set the Leds */
void LEDs_Set(int a, int b, int c)
{
    /* Red Led Off */
    Red_Led_Write(a);
    /* Yellow Led Off */
    Yellow_Led_Write(b);
    /* Green Led Off */
    Green_Led_Write(c);
}

/* Interrupt service routine for UART */
CY_ISR(Rx_Int)
{

    /* Read the received byte from UART and store it in Recived_Byte */
    Received_Byte = UART_1_GetChar();
    UART_1_ClearRxBuffer();
    Rx_Int_ClearPending();

}

/* Function to send a counter value via UART */
void UART_func(int counter) {
    /* Define a character array to hold the string representation of the counter */
    char str[5];
    /* Convert the integer counter to a string and store it in str */
    sprintf(str, "%d", counter);
    /* Send the string via UART */
    UART_1_PutString(str);
    /* Send a carriage return via UART */
    UART_1_PutString("\r");
}

/* Function to start an ADC conversion and return the result */
int ADC_Func() {
    /* Define a signed integer to hold the ADC result */
    signed int result;
    /* Start the ADC conversion */
    ADC_StartConvert();
    /* Wait to end the conversion */
    ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
    /* Get the ADC result */
    result = ADC_GetResult16();
    /* Return the ADC result */
    return result;
}

/* Function to start UART, PWM, and ADC components */
void Start_Components() {
    
    /* Start the Interrupt component */
    Rx_Int_StartEx(Rx_Int);
    
    /* Start the Interrupt component */
    Button_Int_StartEx(Button_Int);
    
    /* Start the UART component */
    UART_1_Start();
    
 
    /* Start the ADC component */
    ADC_Start();
}


/* Function to stop UART, PWM, and ADC components */
void Stop_Components() {
    /* Stop the UART component */
    UART_1_Stop();
    /* Stop the ADC component */
    ADC_Stop();
    /* Green and yellow led off */
    LEDs_Set(1, 0, 0);
    
}

/* Function to show adc values through Leds */
void LED_Detection(int adcValue)
{
    if      (adcValue>=0 && adcValue<=21845)         LEDs_Set(0, 0, 1);
    else if (adcValue>21845 && adcValue<=43690)      LEDs_Set(0, 1, 0);
    else if (adcValue>43690 && adcValue<=65536)      LEDs_Set(1, 0, 0);
}
/* Define the function Byte_Received */
unsigned char Byte_Received() { 
    /* Return the value of Received_Byte */
    return Received_Byte;                         
}
/* Define the function Byte_Received */
void Set_Byte_Received(unsigned char set_byte) { 
    /* Return the value of Received_Byte */
    Received_Byte = set_byte;                         
}

/* [] END OF FILE */
