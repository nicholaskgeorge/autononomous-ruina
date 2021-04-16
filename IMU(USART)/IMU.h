/* 
 * File:   IMU.h
 * Author: daisy
 *
 * Created on April 13, 2021, 11:20 PM
 */

#ifndef IMU_H
#define	IMU_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <stdio.h>
#include "configuration.h"
#include <math.h>
#include <string.h>
#include  <ctype.h> 

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct
{
	// Sentence buffer and associated pointers

	// char _buffer[_bufferLen];
	size_t bufferLen;
	char* buffer;


	// Variables parsed and kept for user
    char messagetype[3];
	double yaw,pitch,roll;
	double MagX,MagY,MagZ;
	double AccelX,AccelY,AccelZ;
    double GyroX,GyroY,GyroZ;

} IMU_INFO ;

bool IMU_Process(IMU_INFO* IMU_info,char* incoming_str);
bool Parse_IMU_Field(IMU_INFO* IMU_info);



#ifdef	__cplusplus
}
#endif

#endif	/* IMU_H */

