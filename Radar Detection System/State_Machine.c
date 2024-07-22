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
#include "project.h"         /* Include project-specific header file */
#include "Drivers.h"         /* Include driver-specific header file */
#include "fft_application.h" /* Include the state machine header file */

#define BUFFER_SIZE 1024     /* Define the size of the ADC buffer */
#define SAMPLE_SIZE 512      /* Define the size of the SAMPLE buffer */



/* Define the possible states of the machine */
typedef enum {
    IDLE,            /* Initialization state */
    SAMPLING,        /* Sampling state */
    UART_TRANSFER,   /* UART transfer state */
} Machine_state;

/* Define a structure to hold machine state and related variables */
typedef struct {
    Machine_state state;           /* State variable */
    uint8 Count;                   /* Counter variable */
    uint16 ADC_Buffer[BUFFER_SIZE]; /* Buffer to store ADC values */
    int32 FFT_Buffer[2*BUFFER_SIZE]; /* Buffer to store ADC values */
    int bufferIndex;               /* Index for the ADC buffer */
} Machine_state_t;

/* Create and initialize a static machine state object */
static Machine_state_t machineState = {IDLE, 0, {0},{0}, 0}; 


/* State machine function */
void StateMachine() {
    
    /* Start all components */
    Start_Components();
    /* Infinite loop to run the state machine */
    while (1) {
        /* State machine switch-case */
        switch (machineState.state) {
            case IDLE:
                    /* Turn on the red LED */
                                  
                    LEDs_Set(1, 0, 0);
                
                break;
            
            case SAMPLING:

                if (Byte_Received() == 's' && machineState.bufferIndex == BUFFER_SIZE) {
                    /* Transition to "UART_TRANSFER" state */
                    machineState.state = UART_TRANSFER;
                    LEDs_Set(0, 0, 0);
                    
                    
                } else {
                    /* Stay in "SAMPLING" state */
                    machineState.state = SAMPLING;
                    /* Turn Off the Red LED and Yellow  */
                    LEDs_Set(0, 0, 1);
                    /* Store ADC result in the buffer */
                    machineState.ADC_Buffer[machineState.bufferIndex] = ADC_Func();                   
                    /* Increment the buffer index and wrap around if it reaches the buffer size */
                    machineState.bufferIndex = (machineState.bufferIndex % BUFFER_SIZE) + 1;
                }
                break;
            
            case UART_TRANSFER:
                /* Check if the count is greater than or equal to 10 and Received_Byte is 'o' */
                if (Byte_Received() == 'o') {
                    /* Increment the count */
                    machineState.Count++;
                    /* Transition to "SAMPLING" state */
                    machineState.state = SAMPLING;
                     /* Initialize the index to zero */
                    machineState.bufferIndex = 0;
                    if (machineState.Count == 10) {
                    /* Transition to "IDLE" state */
                    machineState.state = IDLE;
                    /* Stop all components */
                    Stop_Components();
                    /* Make the count zero again */
                    machineState.Count=0;
                    } 
                } else if (Byte_Received() == 's'){
 
                    /* Send the ADC buffer data over UART */
                    for (int i = 0; i < BUFFER_SIZE; i++) {
                        
                                            /* Send tthe 32bit in 4 transfers */
                    for(int k = 0; k < 2; k++) {
                         UART_1_PutChar(machineState.ADC_Buffer[i]>>8*k);}
                    }
                    
                    
                    fft_app(machineState.ADC_Buffer,machineState.FFT_Buffer,BUFFER_SIZE);
                    /* Send the FFT buffer data over UART */
                    for (int i = 0; i < (2*BUFFER_SIZE); i++) {
                    
                    /* Send tthe 32bit in 4 transfers */
                    for(int k = 0; k < 4; k++) {
                        UART_1_PutChar(machineState.FFT_Buffer[i]>>8*k);
                    }
                    }
                    
                    Set_Byte_Received('0');
                    /* Send the FFT buffer data over UART */
                }
                break;
        }  
    }
}

/* Interrupt service routine for Button */
CY_ISR(Button_Int)
{
    /* Read the received byte from UART and store it in Recived_Byte */
    machineState.state = SAMPLING;
    Start_Components();
    Button_State_Write(1);
}
/* [] END OF FILE */


