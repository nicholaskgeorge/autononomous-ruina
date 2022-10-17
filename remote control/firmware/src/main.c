/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes



/*Calculated duty cycle*/
float duty;
/*Calculated frequency*/
float frequency;
float off_t1,period_t,on_t;

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    uint16_t period, off_time;
    /* Initialize all modules */
    SYS_Initialize ( NULL );

    SYSTICK_TimerStart();
    TC0_CH0_CaptureStart();
      
    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        /* Wait for compare event */
        while((TC0_CH0_CaptureStatusGet() & TC_CAPTURE_B_LOAD) != TC_CAPTURE_B_LOAD);
        
        /* Read Captured values */ 
        off_time = TC0_CH0_CaptureAGet();
        off_t1 = off_time*((853+1/3) * 0.000000001);
        period = TC0_CH0_CaptureBGet();
        period_t = period *((853+1/3) * 0.000000001);
        
        /* Compute Duty Cycle in percentage and Frequency in Hz */
        //on_time = period - off_time;
        on_t = period_t-off_t1;
        duty = ((on_t) / period_t);
        frequency = (TC0_CH0_CaptureFrequencyGet() / period);
        printf("on time,%f,off time,%f period,%f,duty,%f\n",on_t,off_t1,period_t,duty);
        /* Wait for 1 second */ 
        SYSTICK_DelayMs(1000);
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

