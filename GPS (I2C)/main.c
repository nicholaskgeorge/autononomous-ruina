#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <stdio.h>
#include "gps.h"

char buf[500];
size_t nbytes = 500;
GPS_INFO* GPS_info=malloc(sizeof(GPS_INFO*));
int main (void)
{
  SYS_Initialize ( NULL );
  while(true){
    // Read incoming NMEA message packet through USART0
    TWIHS0_Read(0x42, &buf[0], nbytes );
    GPS_Process(GPS_info,(char*)GPS_packet); // parse it into fields
  }

}
