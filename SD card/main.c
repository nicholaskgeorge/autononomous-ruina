#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <stdio.h>
int main ( void )
{
    /* Initialize all modules */
    SDCARD_Initialize();

    char* sensorFiles[3] = {"GPS.txt","Anem.txt","IMU.txt"};
    SDCARD_WriteorRead(false);
    SDCARD_FileName(sensorFiles,3);
    SDCARD_StateSwitch(SDCARD_STATE_CARD_MOUNT);

    while ( true )
    {
       SDCARD_Tasks();
    }
}
