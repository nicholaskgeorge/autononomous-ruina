/*
 * File:   Anemometer.c
 * Anemometer library
 * Author: Daisy Zhang
 *
 * Created on August 26, 2020, 3:38 PM
 */

#include "Anemometer.h"



/*Process the anemometer string and change the Anemometer_info struct
 * accordingly.
 * Input: Anemometer_info: Anemometer_INFO struct pointer
 *        incoming_str: char pointer to the anemometer output
 * Returns: true->process a complete string; false-> otherwise
 */
bool Anem_Process(Anemometer_INFO* Anemometer_info,char* incoming_str){

    char* ptr = incoming_str;
    char* s=ptr;

    while (*ptr=='\0'){
        ++ptr;
    }

    while (*ptr!='\0'){
        if (!isspace(*ptr)&&!isdigit(*ptr)&&*ptr!='.'&&*ptr!='-'){
            s=ptr;
            ptr = ParseField(Anemometer_info,s);
        }
        ++ptr;
    }
    Anemometer_info->bufferLen = sizeof(incoming_str);
    Anemometer_info->buffer = incoming_str;

    if (*ptr == '\0') return true; // Parsed a complete sentence.
    return false;
}

/* Parse the anemometer output and change the Anemometer_info struct accordingly.
 * Input: Anemometer_info: Anemometer_INFO struct pointer
 *        s: char pointer to the start of one field of the anemometer output
 *              string. For example, *s is "S  00.15 ......" or *s could be
 *              "D  078......"
 * Returns: the char pointer which points to the end of one field.
 */
char* ParseField(Anemometer_INFO* Anemometer_info,char* s){

    char* endptr = s+1;
    while (*endptr!='\0'){
        if (!isspace(*endptr)&&!isdigit(*endptr)&&(*endptr!='.')&&(*endptr!='-')){
            if ((*s == 'M'&&*endptr=='D')||(*s == 'P'&& *endptr =='I')||(*s == 'R'&& *endptr =='O')){
                endptr = endptr;
            }else{
                endptr-=1;
                break;
            }
        }
        ++endptr;
    }
    if (*s == 'S'){
        Anemometer_info->speed = strtod (s+2, &endptr);//skip space;
        return endptr;
    }else if (*s == 'D'){
        Anemometer_info->direction = strtod(s+2,&endptr);;//skip space;
        return endptr;
    }else if (*s == 'U'){
        Anemometer_info->u = strtod (s+2, &endptr);//skip space;
        return endptr;
    }else if (*s == 'V'){
        Anemometer_info->v = strtod (s+2, &endptr);//skip space;
        return endptr;
    }else if (*s == 'W'){
        Anemometer_info->w = strtod (s+2, &endptr);//skip space;
        return endptr;
    }else if (*s == 'T'){
        Anemometer_info->temp = strtod (s+2, &endptr);//skip space;
        return endptr;
    }else if (*s == 'H'){
        Anemometer_info->humidity = strtod (s+2, &endptr);//skip space;
        return endptr;
    }else if (*s == 'P'){
        if (*(s+1)!='\0'&&*(s+1)=='I'){
            Anemometer_info->pitch = strtod (s+2, &endptr);//skip space;
            return endptr;
        }else if(*(s+1)!='\0'&&*(s+1)==' '){
            Anemometer_info->pressure =strtod (s+2, &endptr);//skip space;
            return endptr;
        }else
            return endptr;
    }else if (*s == 'R'){
        Anemometer_info->roll = strtod (s+2, &endptr);//skip space;
        return endptr;
    }else if (*s == 'M'){
        Anemometer_info->magnetic_dir = strtod(s+2,&endptr);;//skip space;
        return endptr;
    }
    return endptr;

}
