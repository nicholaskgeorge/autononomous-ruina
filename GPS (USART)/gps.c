/* 
 * File:   gps.c
 * GPS library
 * Author: Daisy Zhang
 *
 * Created on August 23, 2020, 19:25 PM
 */


#include "gps.h"

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <stdio.h>
#include <math.h>
#include <string.h>
#include  <ctype.h> 
#include "./default/peripheral/usart/plib_usart0.h"

/* Setup the buffer storing GPS output string.
* Input: GPS_info: GPS_INFO struct pointer
*        buf: empty buffer
*        len: length of the buffer
*/
void setGPSBuffer(GPS_INFO* GPS_info,void* buf, uint8_t len){
    
	GPS_info->bufferLen = len;
	GPS_info->buffer = (char*)buf;
	GPS_info->ptr = GPS_info->buffer;
	if (GPS_info->bufferLen) {
		*GPS_info->ptr = '\0';
		GPS_info->buffer[GPS_info->bufferLen - 1] = '\0';
	}
}

/* Parse the GPS output
* Input: 
*        s: char pointer pointing to the start of one field of GPS string 
*        result: char pointer pointing to where stores the field value
*        len: length of the result storage buffer 
* Returns: the char pointer which points to the end of one field.
*/
const char* parseField(const char* s, char *result, int len){

	if (s == NULL)
		return NULL;

	int i = 0;
	while (*s != ',' && *s != '\0') {
		if (result && i++ < len)
			*result++ = *s;
		++s;
	}
	if (result && i < len)
		*result = '\0'; // Terminate unless too long

	if (*s == ',')
		return ++s; // Location of start of next field
	else
		return NULL; // End of string or valid sentence
}

/* Skip any field of the GPS output
* Input: 
*        s: char pointer pointing to ',', the end of last field
* Returns: the char pointer which points to the start of next field.
*/
const char* skipField(const char* s)
{   
	if (s == NULL)
		return NULL;

	while (*s != '\0') {
		if (*s == ',') {
			// Check next character
			if (*s++ != '\0')
				break;
			else
				return s;
		}
		++s;
	}
	return NULL; // End of string or valid sentence
}

/* Convert char* to unsigned integer
* Input: 
*        s: char pointer pointing to digit character
*       len: length of the digit string
* Returns: the value of the digit string representing for 
*/
unsigned int parseUnsignedInt(const char *s, uint8_t len)
{
    
	int r = 0;
	while (len--)
		r = 10 * r + *s++ - '0';
	return r;
}

/* Parse output and update GPS_info accordingly.
* Input: 
*        GPS_info: GPS_INFO pointer struct
*       s: the char pointer which points to the start of time field
* Returns: char  pointer which points to the start of next field
*/
const char* parseTime(GPS_INFO* GPS_info,const char* s)
{
	if (*s == ',')
		return skipField(s);
	GPS_info->hour = parseUnsignedInt(s, 2);
	GPS_info->minute = parseUnsignedInt(s + 2, 2);
	GPS_info->second = parseUnsignedInt(s + 4, 2);
	GPS_info->hundredths = parseUnsignedInt(s + 7, 2);
	return skipField(s + 9);
}

/* Parse output and update GPS_info accordingly.
* Input: 
*        GPS_info: GPS_INFO pointer struct
*       s: the char pointer which points to the start of date field
* Returns: char  pointer which points to the start of next field
*/
const char* parseDate(GPS_INFO* GPS_info,const char* s)
{
	if (*s == ',')
		return skipField(s);
	GPS_info->day = parseUnsignedInt(s, 2);
	GPS_info->month = parseUnsignedInt(s + 2, 2);
	GPS_info->year = parseUnsignedInt(s + 4, 2) + 2000;
	return skipField(s + 6);
}

/* Calculates power of 10 with exponent = b
 * Input:
 *      b: the exponent
 * Returns: power of 10 with exponent b */
long exp_10(uint8_t b)
{
	long r = 1;
	while (b--)
		r *= 10;
	return r;
}

/* Convert char* to float
* Input: 
*        s: char pointer pointing to digit character
*      log10Multiplier: the exponent of power of 10
*       eptr: points to the addess of the pointer pointing to the end of the field
* Returns: the value of the digit string representing for 
*/
long parseFloat(const char* s, uint8_t log10Multiplier, const char** eptr)
{
	int8_t neg = 1;
	long r = 0;
	while (isspace(*s))
		++s;
	if (*s == '-') {
		neg = -1;
		++s;
	}
	else if (*s == '+')
		++s;

	while (isdigit(*s))
		r = 10*r + *s++ - '0';
	r *= exp_10(log10Multiplier);

	if (*s == '.') {
		++s;
		long frac = 0;
		while (isdigit(*s) && log10Multiplier) {
			frac = 10 * frac + *s++ -'0';
			--log10Multiplier;
		}
		frac *= exp_10(log10Multiplier);
		r += frac;
	}
	r *= neg; // Include effect of any minus sign

	if (eptr)
		*eptr = skipField(s);
	return r;
}

/* Convert the input string to degree minute
* Input: 
*        
*       s: the char pointer which points to the start of field
*      degWidth: the length of degree string
*       eptr: points to the addess of the pointer pointing to the end of the field
* Returns: the value in degree minute
*/
long parseDegreeMinute(const char* s, uint8_t degWidth,
								  const char **eptr)
{
	if (*s == ',') { 
		if (eptr)
			*eptr = skipField(s);
		return 0;
	}
	long r = parseUnsignedInt(s, degWidth) * 1000000L;
	s += degWidth;
	r += parseFloat(s, 6, eptr) / 60;
	return r;
}

/* Parse GGA message and update GPS_info accordingly
 * Input: 
 *      GPS_info: struct pointer
 *      s: points to the start of GGA message
 * Returns: true-> process a complete GGA message, false-> otherwise
 */
bool processGGA(GPS_INFO* GPS_info,const char *s)
{
	// Process GGA format messages. 
	GPS_info->navSystem = GPS_info->talkerID;

	s = parseTime(GPS_info,s);
	// ++s;
	GPS_info->latitude = parseDegreeMinute(s, 2, &s);
	if (*s == ',')
		++s;
	else {
		if (*s == 'S' )
			GPS_info->latitude *= -1;
		s += 2; // Skip N/S and comma
	}
	GPS_info->longitude = parseDegreeMinute(s, 3, &s);
	if (*s == ',')
		++s;
	else {
		if (*s == 'W')
			GPS_info->longitude *= -1;
		s += 2; // Skip E/W and comma
	}
	GPS_info->isValid = (*s == '1' || *s == '2');
	s += 2; // Skip position fix flag and comma
	GPS_info->numSat = parseFloat(s, 0, &s);
	GPS_info->hdop = parseFloat(s, 1, &s);
	GPS_info->altitude = parseFloat(s, 3, &s);
	GPS_info->altitudeValid = true;
	// That's all we care about
	return true;
}

/* Parse RMC message and update GPS_info accordingly
 * Input: 
 *      GPS_info: struct pointer
 *      s: points to the start of RMC message
 * Returns: true-> process a complete RMC message, false-> otherwise
 */
bool processRMC(GPS_INFO* GPS_info,const char* s)
{
	// RMC format message
	GPS_info->navSystem = GPS_info->talkerID;

	s = parseTime(GPS_info,s);
	GPS_info->isValid = (*s == 'A');
	s += 2; // Skip validity and comma
	GPS_info->latitude = parseDegreeMinute(s, 2, &s);
	if (*s == ',')
		++s;
	else {
		if (*s == 'S')
			GPS_info->latitude *= -1;
		s += 2; // Skip N/S and comma
	}
	GPS_info->longitude = parseDegreeMinute(s, 3, &s);
	if (*s == ',')
		++s;
	else {
		if (*s == 'W')
			GPS_info->longitude *= -1;
		s += 2; // Skip E/W and comma
	}
	GPS_info->speed = parseFloat(s, 3, &s);
	GPS_info->course = parseFloat(s, 3, &s);
	s = parseDate(GPS_info,s);
	// That's all we care about
	return true;
}

/* Parse NMEA message and update GPS_info accordingly
 * Input: 
 *      GPS_info: struct pointer
 *      incoming_str: points to the start of NMEA string
 * Returns: true-> process a complete NMEA message, false-> otherwise
 */
bool NMEA_Process(GPS_INFO* GPS_info,char* incoming_str)
{
    
    char* ptr = incoming_str;
    

    while (*ptr!='\0'){
        if (*ptr == '$') {
            char* endptr = ptr;
            while (*endptr!='\0'){
                if (*endptr == '\r')
                    break;
                ++endptr;
            }

            // Valid message
            const char* data;
            if (ptr[1] == 'G') {
                GPS_info->talkerID = ptr[2];
                data = parseField(&ptr[3], &GPS_info->messageID[0], sizeof(GPS_info->messageID));
            }
            else {               
                GPS_info->talkerID = '\0';
                data = parseField(&ptr[1], &GPS_info->messageID[0], sizeof(GPS_info->messageID));
            }

            if (strcmp(&GPS_info->messageID[0], "GGA") == 0)
                return processGGA(GPS_info,data);
            else if (strcmp(&GPS_info->messageID[0], "RMC") == 0)
                return processRMC(GPS_info,data);
            ptr = endptr;
        }
        ptr++;
    }
    return false;
}
