#ifndef _IMU_H    /* Guard against multiple inclusion */
#define _IMU_H


#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define buff_size 11
    
typedef struct
{
	// Sentence buffer and associated pointers

	// char _buffer[_bufferLen];
	size_t bufferLen;
	double yaw,pitch,roll;
//	double MagX,MagY,MagZ;
//	double AccelX,AccelY,AccelZ;
//    double GyroX,GyroY,GyroZ;
//    double heading;

} NEW_IMU_INFO ;



extern NEW_IMU_INFO* new_imu_info;

void New_IMU_Process(NEW_IMU_INFO* IMU_info, uint8_t* data);


#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

