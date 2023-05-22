#include "definitions.h"
#include "read_new_imu.h"
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h> 

void New_IMU_Process(NEW_IMU_INFO* IMU_info, uint8_t* data){
   uint8_t* ptr = data;
   uint8_t low_byte = 0;
   uint8_t high_byte = 0;

   //move past the header
   ptr += 2;

   //Roll
   low_byte = *ptr;
   high_byte = *(ptr+1);
   IMU_info->roll = low_byte | high_byte<<8;
   //move to next value
   ptr += 2;

   //Pitch
   low_byte = *ptr;
   high_byte = *(ptr+1);
   IMU_info->pitch = low_byte | high_byte<<8;
   //move to next value

   //Yaw
   low_byte = *ptr;
   high_byte = *(ptr+1);
   IMU_info->yaw = low_byte | high_byte<<8;
   //move to next value
   ptr += 2;

   //the last bytes of the message contain the tempurature and the
   //check sum which we do not care about

}