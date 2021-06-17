/*
 * File:   radio.c
 * radio library
 * Author: Nicholas George
 *
 * Created on June 15, 2021 9:26 PM
 */

#include "radio.h"

/* Decodes incoming radio messages. Look at WIZE manual for more details
 * Input: array or pointer to array of data, pointer to buffer and size of data
 *        data: array of bytes to send over serial from radio module
 *        buffer: place to hold the message. Must be 4 bytes longer than message
 *        size: size of the data which is being sent
 * Returns: the length of the message as an int.
 * Note: the maximum size message the radio can send is 0xF3 or 243 bytes
 * (including the package bytes its 0xF6 or 246) so size your messages accordingly
 */
int Radio_Encode(uint8_t* data, uint8_t* buffer, int size){
    int lengthbit = size+3;
    *buffer = lengthbit;
    *(buffer+1) = 0x20;
    *(buffer+3+size) = 0x00;
    *(buffer+2+size) = 0x00;
    memcpy((buffer+2), data, size);
    return size+4;
}

/* Decodes incoming radio messages. Look at WIZE manual for more details
 * Input: array or pointer to array of type uint8_t
 *        data: array of bytes read over serial from radio module
 *        buffer: place to hold the message. Must be long enough to hold message
 * Returns: the length of the message as an int.
 */
int Radio_Decode(uint8_t *data, uint8_t *buffer){
    //getting pointer to actual message
    uint8_t *ptr =  data;
    //calculating length of important data. Length of whole message is at index 0
    uint8_t length_message = *ptr-10;
    //message does not start until 11 bytes in
    ptr+=11;
    //Copying message to buffer
    memcpy(buffer, ptr, length_message);
    return (int)length_message;
}
