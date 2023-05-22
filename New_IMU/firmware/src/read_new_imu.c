#include "definitions.h"
#include "read_new_imu.h"
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h> 


double imu_process(uint8_t low_byte, uint8_t high_byte){
    double val1 = high_byte<<8 | low_byte;
    double val2 = val1/32768*180;
    return val2;
}

void New_IMU_Process(NEW_IMU_INFO* IMU_info, uint8_t* data){
   uint8_t* ptr = data;
   uint8_t low_byte = 0;
   uint8_t high_byte = 0;

   //move past the header
   ptr += 2;
   
   
   //Roll
   low_byte = *ptr;
   high_byte = *(ptr+1);
   IMU_info->roll = imu_process(low_byte, high_byte);
   //move to next value
   ptr += 2;

   //Pitch
   low_byte = *ptr;
   high_byte = *(ptr+1);
   IMU_info->pitch = imu_process(low_byte, high_byte);
   ptr += 2;
   //move to next value

   //Yaw
   low_byte = *ptr;
   high_byte = *(ptr+1);
   IMU_info->yaw = imu_process(low_byte, high_byte);

   //the last bytes of the message contain the tempurature and the
   //check sum which we do not care about

}

