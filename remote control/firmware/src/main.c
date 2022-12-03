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
#include "decode_ibus.h"



#define receive_buffer_size 64
uint8_t rec_buffer[receive_buffer_size];
uint8_t message[32];
int channels[14];


/*Calculated duty cycle*/
float duty1,duty2;
/*Calculated frequency*/
float frequency1,frequency2;
uint16_t on_time1,on_time2;
float angle1,angle2;
// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

float duty2angle(int K, float duty)
{
    float angle;
    angle = 2*K*duty-K;
    return angle;
}

//int map(int i_max, int i_min, int o_max, int o_min){
//    float slope = (o_max-o_min)/(i_max-i_min);
//    float y_int = 
//}

int main ( void )
{
//    uint16_t period1, off_time1;
//    uint16_t period2, off_time2;
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    TC0_CH0_CaptureStart();
    TC3_CH2_CaptureStart();
    
    USART2_Initialize();
      
    while ( true )
    {
//        /* Maintain state machines of all polled MPLAB Harmony modules. */
//        /* Wait for compare event */
//        /*Channel 1 duty cycle calculation*/
//        while((TC0_CH0_CaptureStatusGet() & TC_CAPTURE_B_LOAD) != TC_CAPTURE_B_LOAD);
//       
//        /* Read Captured values */ 
//        off_time1 = TC0_CH0_CaptureAGet();
//        period1 = TC0_CH0_CaptureBGet();
//                
//        /* Compute Duty Cycle in percentage and Frequency in Hz */
//        //on_time = period - off_time;
//        on_time1 = period1-off_time1;
//        duty1 = ((on_time1) / period1);
//        frequency1 = (TC0_CH0_CaptureFrequencyGet() / period1);
//        printf("on time1,%f,off time1,%d period1,%d,duty1,%f\n",on_time1,off_time1,period1,duty1);
//        angle1 = duty2angle(175,duty1);
//        printf("angle for sail,%f\n",angle1);
//        
//        /*Channel 2 duty cycle calculation*/
//        while((TC3_CH2_CaptureStatusGet() & TC_CAPTURE_B_LOAD) != TC_CAPTURE_B_LOAD);
//        off_time2 = TC3_CH2_CaptureAGet();
//        period2 = TC3_CH2_CaptureBGet();
//        /* Compute Duty Cycle in percentage and Frequency in Hz */
//        //on_time = period - off_time;
//        on_time2 = period2-off_time2;
//        duty2 = ((on_time2) / period2);
//        frequency2 = (TC3_CH2_CaptureFrequencyGet() / period2);
//        printf("on time2,%f,off time2,%d period2,%d,duty2,%f\n",on_time2,off_time2,period2,duty2);
//        angle2 = duty2angle(15,duty2);
//        printf("angle for rudder,%f\n",angle2);
//        
//        USART2_Read(&rec_buffer, receive_buffer_size );
        if(USART2_Read(&rec_buffer, receive_buffer_size ))
        {
          
            for(int channel_index=0;channel_index<64;channel_index++){
                if(rec_buffer[channel_index]==0x20){
                    for(int a=0;a<32;a++){
                        message[a]=rec_buffer[channel_index+a];
                    }
                    break;
                }
                break;
            }
//            for (int i=0;i<32;i++){
//                USART1_Write(&message[i],sizeof(message[i]));
//            }
//
//            for (int j=0;j<64;j++){
//                USART1_Write(&rec_buffer[j],sizeof(rec_buffer[j]));
//            }

        }
//        
//        int test[2] = {0xB6,5};
//        int c1 = *test;
//        int c2 = *(test+1);
//        c2+=0;
//        c1+=0;
        get_channels(message, channels);
//        for (int i=0;i<14;i++){
//            USART1_Write(&channels[i],sizeof(channels[i]));
//        }
        on_time1 = channels[0];
        on_time2 = channels[2];
//        USART1_Write(&on_time1,sizeof(on_time1));
        USART1_Write(&on_time2,sizeof(on_time2));
        duty1 = ((float)on_time1-1000)/1000;
        duty2 = ((float)on_time2-1000)/1000;
//        printf("duty2:%f\n",duty2);
        angle1 = duty2angle(175,duty1);
        printf("angle for sail,%f\n",angle1);
//        angle2 = duty2angle(15,duty2);
//        printf("angle for rudder,%f\n",angle2);
        
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

