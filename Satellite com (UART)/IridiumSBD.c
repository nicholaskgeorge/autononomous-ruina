/*
 * File:  IridiumSBD.c
 * Satellite communication library
 * Author: Daisy Zhang
 *
 * Created on August 26, 2020, 3:38 PM
 */

#include "IridiumSBD.h"

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <stdio.h>
#include <math.h>
#include <string.h>
#include  <ctype.h>
#include "./default/peripheral/uart/plib_uart0.h"

/* Write to IridiumSBD
 * Input: buffer - contents to write
 * Returns: if the operation succeeds
 */
bool send( void *buffer){
  bool status = UART0_Write(buffer, sizeof(buffer));
  return status;
}

/*
Adjust time out in seconds.
Defalult value is 20 seconds.
 */
void adjustATTimeout(IridiumSBD* self,,int seconds){
  self->atTimeout = seconds;
}


// Last time edition 

/*
Wait for response from previous AT command.
This process terminates when "terminator" string is seen or upon timeout.
If "prompt" string is provided (example "+CSQ:"), then all characters following
prompt up to the next CRLF are stored in response buffer for later parsing by caller.
 */
bool waitForATResponse(IridiumSBD* self,char *response=NULL, int responseSize=0, const char *prompt=NULL, const char *terminator="OK\r\n"){
   
}

/*
Disable flow control
Input: void
Returns: if the operation succeeds
 */
bool disableFlowControl(void){
  bool status = send("AT&K0\r");
  return status 
}

int sendSBDText(IridiumSBD* self,const char *message){


}
int sendSBDBinary(IridiumSBD* self,const uint8_t *txData, size_t txDataSize){
   send("AT+SBDWB=[");
   send(txDataSize);// todo : int to string
   send("]\r");

   // wait for READY\r
   
   // send txData + checksum 
   // wait for 0\r\n\r\nOK\r\n
}
int sendReceiveSBDText(IridiumSBD* self,const char *message, uint8_t *rxBuffer, size_t &rxBufferSize){

}
int sendReceiveSBDBinary(IridiumSBD* self,const uint8_t *txData, size_t txDataSize, uint8_t *rxBuffer, size_t &rxBufferSize){

}
int getSignalQuality(IridiumSBD* self,int &quality){
}
int getSystemTime(IridiumSBD* self,struct tm &tm){
}
int getFirmwareVersion(IridiumSBD* self,char *version, size_t bufferSize){
}
int getWaitingMessageCount(IridiumSBD* self){
}
bool isAsleep(IridiumSBD* self){
}
bool hasRingAsserted(IridiumSBD* self){
}
int sleep(IridiumSBD* self){

}
