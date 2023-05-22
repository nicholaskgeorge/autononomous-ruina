#include "definitions.h"
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h> 
#include "read_new_imu.h"

uint8_t data_buffer[buff_size];
NEW_IMU_INFO* new_imu_info;
int save_val;
double yaw;

int main ( void )
{
    SYS_Initialize ( NULL );
    while(1){
        USART1_Read(data_buffer, buff_size);
        New_IMU_Process(new_imu_info, data_buffer);
        yaw = new_imu_info->yaw;
        printf("Yaw:%f ,Pitch:%f, Roll:%f\n" , new_imu_info->yaw, new_imu_info->pitch, new_imu_info->roll);
    }
}


/*******************************************************************************
 End of File
*/

