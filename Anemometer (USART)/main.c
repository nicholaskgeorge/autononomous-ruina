#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <stdio.h>
#include "Anemometer.h"

Anemometer_INFO* Anemometer_info;
uint8_t Anemometer_values[101];

int main (void)
{
  SYS_Initialize ( NULL );
  while(true){
    // Read incoming message packet through USART1
    USART1_Read(Anemometer_values,sizeof(Anemometer_values));
    Anem_Process(Anemometer_info,(char*)Anemometer_values); // parse it into fields
  }

}
