#include "definitions.h"
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h> 
#include "read_new_imu.h"

uint8_t data_buffer[buff_size];

int main ( void )
{
    USART1_Read(data_buffer, buff_size);
    New_IMU_Process(new_imu_info, data_buffer);
    printf("%f" , new_imu_info->yaw);
}


/*******************************************************************************
 End of File
*/

