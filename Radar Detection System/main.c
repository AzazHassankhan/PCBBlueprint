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
#include "stdio.h"         /* Include standard I/O library */
#include "State_Machine.h" /* Include the state machine header file */

int main(void) {
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    StateMachine(); /* Call the state machine function. */
}

/* [] END OF FILE */
