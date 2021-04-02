#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <stdio.h>
int main ( void )
{
  /* SD Card code work initialize Starts */
  SDCARD_DATA* SdcardData=malloc(sizeof(SDCARD_DATA*));
  SDCARD_Initialize(SdcardData);
  char* sensorFiles[3] = {"GPS.txt","Anem.txt","IMU.txt"};
  SDCARD_WriteorRead(SdcardData,true);
  SDCARD_FileName(SdcardData,sensorFiles[0]);
  SDCARD_StateSwitch(SdcardData,SDCARD_STATE_CARD_MOUNT);
  /* SD Card code work initialize Ends */

  uint8_t buf[500];
  size_t nbytes = 500;

    while ( true )
    {

      /* Never call SDCARD_Tasks on one SDCARD_DATA pointer more than once in one loop*/

      /* I2C data logging Starts */
       TWIHS0_Read(0x42, &buf[0], nbytes );
       if (!isallspace(buf,nbytes)){
           Buffer b = remove_spaces(buf, nbytes);
           SDCARD_FillinBuffer(SdcardData,b.ptr,b.len);
           SDCARD_Tasks(SdcardData);
       }
       /* I2C data logging Ends */

       /* OR */

       /* USART data logging Starts */
        USART2_Read(&buf[0],nbytes);
        SDCARD_FillinBuffer(SdcardData,buf,nbytes);
        SDCARD_Tasks(SdcardData);
        /* USART data logging Ends */
    }
}
