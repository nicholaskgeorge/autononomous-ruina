/* 
 * File:   gps.h
 * GPS library
 * Author: Daisy Zhang
 *
 * Created on August 23, 2020, 4:47 PM
 */

#ifndef GPS_H
#define	GPS_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <stdio.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct
{
	// Sentence buffer and associated pointers

	// char _buffer[_bufferLen];
	uint8_t bufferLen;
	char* buffer;
	char *ptr;

	// Information from current NMEA sentence
	char talkerID;
	char messageID[6];

	// Variables parsed and kept for user
	char navSystem;
	bool isValid;
	long latitude, longitude; // In millionths of a degree
	long altitude; // In millimetres
	bool altitudeValid;
	long speed, course;
	uint16_t year;
	uint8_t month, day, hour, minute, second,hundredths;
	uint8_t numSat;
	uint8_t hdop;




} GPS_INFO ;

void setGPSBuffer(GPS_INFO* GPS_info,void* buf, uint8_t len);
bool NMEA_Process(GPS_INFO* GPS_info,char* incoming_char);
const char* parseField(const char* s, char *result, int len);
bool processGGA(GPS_INFO* GPS_info,const char *s);
bool processRMC(GPS_INFO* GPS_info,const char* s);
const char* parseTime(GPS_INFO* GPS_info,const char* s);
const char* parseDate(GPS_INFO* GPS_info,const char* s);
const char* skipField(const char* s);
unsigned int parseUnsignedInt(const char *s, uint8_t len);
long parseDegreeMinute(const char* s, uint8_t degWidth,const char **eptr);
long parseFloat(const char* s, uint8_t log10Multiplier, const char** eptr);
long exp_10(uint8_t b);
#ifdef	__cplusplus
}
#endif

#endif	/* GPS_H */

