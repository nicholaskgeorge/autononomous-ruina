#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <stdio.h>
#include "IridiumSBD.h"


uint8_t SBDFeedback[50];
uint8_t* SBDF=SBDFeedback;

int main (void)
{
  SYS_Initialize ( NULL );
  while(true){
    // write to IridiumSBD
    UART0_Write("AT+SBDIX\r",sizeof("AT+SBDIX\r"));
    if ( UART0_ReceiverIsReady())
    UART0_Read(SBDFeedback,sizeof(SBDFeedback));

    uint8_t a = UART0_ReadByte();
    while(a!=0x0A){
        *SBDF=a;
        SBDF++;
        a = UART0_ReadByte();
    }
  }

}
