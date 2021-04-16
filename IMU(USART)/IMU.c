/* 
 * File:   IMU.c
 * IMU library
 * Author: Daisy Zhang
 *
 * Created on April 13, 2021,
 */

#include "IMU.h"

bool IMU_Process(IMU_INFO* IMU_info,char* incoming_str){
    
    char* ptr = incoming_str;
    char* start=ptr;
    char* end = ptr;
    
    while (*ptr=='\0'){ // find the start of non-null values
        ++ptr;
    }
    
    while (*ptr != '$' && *ptr != '\0'){ // find the start of the message
        ++ptr;
    }
    if (*ptr == '\0'){
        return false;
    }else{
        start = ptr;
    }
    
    while (*ptr != '*' && *ptr!='\0'){ // find the end of the message
        ++ptr;
    }
    if (*ptr == '\0'){
        return false;
    }else{
        end = ptr;
    }
    
    IMU_info->bufferLen = end-start;
    IMU_info->buffer = start;
    
    
    
    return Parse_IMU_Field(IMU_info);
    
}

bool Parse_IMU_Field(IMU_INFO* IMU_info){
    char* str=IMU_info->buffer;
    char* end = str+IMU_info->bufferLen;
 
    char * pch;
    long int field_index[12];
    int i = 0;
    
    pch=strchr(str,',');
    while (pch!=NULL && pch-end<=0)
    {
        field_index[i] = pch-str;
        pch=strchr(pch+1,',');
        i++;
    }
    if (i!=12){
        return false;
    }
    strncpy(IMU_info->messagetype, str+3, 3);

    char* endptr = str+field_index[1]-1;
    IMU_info->yaw = strtod (str+1+field_index[0], &endptr);
    endptr = str+field_index[2]-1;
    IMU_info->pitch = strtod (str+1+field_index[1], &endptr);
    endptr = str+field_index[3]-1;
    IMU_info->roll = strtod (str+1+field_index[2], &endptr);
    endptr = str+field_index[4]-1;
    IMU_info->MagX = strtod (str+1+field_index[3], &endptr);
    endptr = str+field_index[5]-1;
    IMU_info->MagY = strtod (str+1+field_index[4], &(endptr));
    endptr = str+field_index[6]-1;
    IMU_info->MagZ = strtod (str+1+field_index[5], &endptr);
    endptr = str+field_index[7]-1;
    IMU_info->AccelX = strtod (str+1+field_index[6], &endptr);
    endptr = str+field_index[8]-1;
    IMU_info->AccelY = strtod (str+1+field_index[7], &endptr);
    endptr = str+field_index[9]-1;
    IMU_info->AccelZ = strtod (str+1+field_index[8], &(endptr));
    endptr = str+field_index[10]-1;
    IMU_info->GyroX = strtod (str+1+field_index[9], &endptr);
    endptr = str+field_index[11]-1;
    IMU_info->GyroY = strtod (str+1+field_index[10], &endptr);
    endptr = end-1;
    IMU_info->GyroZ = strtod (str+1+field_index[11], &endptr);

    return true;
}