#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <stdio.h>
#include "gps.h"

uint8_t GPS_packet[100];
GPS_INFO* GPS_info;

int main (void)
{
  SYS_Initialize ( NULL );
  while(true){
    // Read incoming NMEA message packet through USART0
    USART0_Read(GPS_packet,sizeof(GPS_packet));
    NMEA_Process(GPS_info,(char*)GPS_packet); // parse it into fields
  }

}
