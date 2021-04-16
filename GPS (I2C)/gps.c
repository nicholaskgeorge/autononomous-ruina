/*
 * File:   gps.c
 * GPS library
 * Author: Daisy Zhang
 *
 * Created on August 23, 2020, 19:25 PM
 */


#include "gps.h"

/* Parse output and update GPS_info accordingly.
* Input:
*        GPS_info: GPS_INFO pointer struct
*       s: the char pointer which points to the start of time field
* Returns: char  pointer which points to the start of next field
*/
 char* parseTime(GPS_INFO* GPS_info, char* s)
{
	if (*s == ',')
		return s+1;
    char* num = malloc(sizeof(char)*2+1);
    strncpy(num, s, 2);
	GPS_info->hour = atoi(num);
    strncpy(num, s+2, 2);
	GPS_info->minute = atoi(num);
    strncpy(num, s+4, 2);
	GPS_info->second = atoi(num);
    strncpy(num, s+7, 2);
	GPS_info->hundredths = atoi(num);
	return s+10;
}

/* Parse output and update GPS_info accordingly.
* Input:
*        GPS_info: GPS_INFO pointer struct
*       s: the char pointer which points to the start of date field
* Returns: char  pointer which points to the start of next field
*/
 char* parseDate(GPS_INFO* GPS_info, char* s)
{
    if (*s == ',')
        return s+1;

    char* num = malloc(sizeof(char)*2+1);
    strncpy(num, s, 2);
    GPS_info->day = atoi(num);
    strncpy(num, s+2, 2);
    GPS_info->month =  atoi(num);
    strncpy(num, s+4, 2);
    GPS_info->year =  atoi(num) + 2000;
    return (s + 7);
}


/* Convert char* to float
* Input:
*        s: char pointer pointing to digit character
*       eptr: points to the addess of the pointer pointing to the end of the field
* Returns: the value of the digit string representing for
*/
double parseFloat( char* s,   char** eptr)
{
    if (*s == ',') {
        if (eptr)
            *eptr = s+1;
        return 0;
    }
    char* end;

    double parsed_val =  strtod (s, &end);
    *eptr = end+1;
    return parsed_val;
}

/* Convert the input string to degree minute
* Input:
*
*       s: the char pointer which points to the start of field
*      degWidth: the length of degree string
*       eptr: points to the address of the pointer pointing to the end of the field
* Returns: the value in degree minute
*/
double parseDegreeMinute( char* s, uint8_t degWidth,
								   char **eptr)
{
	if (*s == ',') {
		if (eptr)
			*eptr = s+1;
		return 0;
	}

    char* num = malloc(sizeof(char)*2+1);
    strncpy(num, s, degWidth);
	double r = atof(num);
	s += degWidth;
    char* end = s+7;
	r += strtod(s, &end)/60;
    *eptr = end+2;
	return r;
}

/* Parse GGA message and update GPS_info accordingly
 * Input:
 *      GPS_info: struct pointer
 *      s: points to the start of GGA message
 * Returns: true-> process a complete GGA message, false-> otherwise
 */
bool processGGA(GPS_INFO* GPS_info, char *start)
{
	// Process GGA format messages.
	GPS_info->navSystem = GPS_info->talkerID;

     char* s = start;
    s=strchr(start,',');
	s = parseTime(GPS_info,s+1);

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

    if (!GPS_info->isValid ){
        return false;
    }
	s += 2; // Skip position fix flag and comma
	GPS_info->numSat = (int)parseFloat(s,&s);
	GPS_info->hdop = parseFloat(s,  &s);
	GPS_info->altitude = parseFloat(s,  &s);
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
bool processRMC(GPS_INFO* GPS_info, char* start)
{
	// RMC format message
	GPS_info->navSystem = GPS_info->talkerID;

     char* ptr = start;

    ptr=strchr(start,',');
	ptr = parseTime(GPS_info,ptr+1);


	GPS_info->isValid = (*ptr == 'A');

    if (!GPS_info->isValid ){
        return false;
    }
	ptr += 2; // Skip validity and comma

	GPS_info->latitude = parseDegreeMinute(ptr, 2, &ptr);

	if (*ptr == ',')
		++ptr;
	else {
		if (*ptr == 'S')
			GPS_info->latitude *= -1;
		ptr += 2; // Skip N/S and comma
	}

	GPS_info->longitude = parseDegreeMinute(ptr, 3, &ptr);
	if (*ptr == ',')
		++ptr;
	else {
		if (*ptr == 'W')
			GPS_info->longitude *= -1;
		ptr += 2; // Skip E/W and comma
	}

	GPS_info->speed = parseFloat(ptr, &ptr);
	GPS_info->course = parseFloat(ptr,&ptr);
	ptr = parseDate(GPS_info,ptr);
	// That's all we care about
	return true;
}

/* Parse NMEA message and update GPS_info accordingly
 * Input:
 *      GPS_info: struct pointer
 *      incoming_str: points to the start of NMEA string
 * Returns: true-> process a complete NMEA message, false-> otherwise
 */
bool GPS_Process(GPS_INFO* GPS_info,char* incoming_str)
{

    char* ptr = incoming_str;
    char* start = ptr;
    char* end = ptr;

    bool result[2] = {false,false};

    while (*ptr=='\0'){ // find the start of non-null values
        ++ptr;
    }

    char msg[3];

    while (*ptr != '\0'){ // find the start of the message
        if (*ptr == '$'){
            start = ptr;
            end = start;
            while (*end!='\0'){
                if (*end == '\r')
                    break;
                ++end;
            }

            if (*end == '\0'){
                return result[0]&&result[1];
            }

            GPS_info->bufferLen = end-start;
            GPS_info->buffer = start;

            GPS_info->talkerID = start[2];

            strncpy(msg, start+3, 3);
            strcpy(GPS_info->messageID, msg);
            if (strcmp(msg, "GGA") == 0) // maybe unnecessary
                result[0] = processGGA(GPS_info,start);
            else if (strcmp(msg, "RMC") == 0)
                result[1] = processRMC(GPS_info,start);

            ptr = end;
        }
        ++ptr;
    }
    return result[0]&&result[1];
}
