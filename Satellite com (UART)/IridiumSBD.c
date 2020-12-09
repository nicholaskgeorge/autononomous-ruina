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
bool ConsoleWrite( void *buffer){
  bool status = UART_Write(buffer, sizeof(buffer));
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
  diagprint(F("Waiting for response "));
     diagprint(terminator);
     diagprint(F("\r\n"));

     if (response)
        memset(response, 0, responseSize);

     int matchPromptPos = 0; // Matches chars in prompt
     int matchTerminatorPos = 0; // Matches chars in terminator
     enum {LOOKING_FOR_PROMPT, GATHERING_RESPONSE, LOOKING_FOR_TERMINATOR};
     int promptState = prompt ? LOOKING_FOR_PROMPT : LOOKING_FOR_TERMINATOR;
     consoleprint(F("<< "));
     for (unsigned long start=millis(); millis() - start < 1000UL * atTimeout;)
     {
        if (cancelled())
           return false;

        while (filteredavailable() > 0)
        {
           char c = filteredread();
           if (prompt)
           {
              switch (promptState)
              {
              case LOOKING_FOR_PROMPT:
                 if (c == prompt[matchPromptPos])
                 {
                    ++matchPromptPos;
                    if (prompt[matchPromptPos] == '\0')
                       promptState = GATHERING_RESPONSE;
                 }

                 else
                 {
                    matchPromptPos = c == prompt[0] ? 1 : 0;
                 }

                 break;
              case GATHERING_RESPONSE: // gathering response from end of prompt to first \r
                 if (response)
                 {
                    if (c == '\r' || responseSize < 2)
                    {
                       promptState = LOOKING_FOR_TERMINATOR;
                    }
                    else
                    {
                       *response++ = c;
                       responseSize--;
                    }
                 }
                 break;
              }
           }

           if (c == terminator[matchTerminatorPos])
           {
              ++matchTerminatorPos;
              if (terminator[matchTerminatorPos] == '\0')
                 return true;
           }
           else
           {
              matchTerminatorPos = c == terminator[0] ? 1 : 0;
           }
        } // while (stream.available() > 0)
     } // timer loop
     return false;
}

/*
Disable flow control
Input: void
Returns: if the operation succeeds
 */
bool disableFlowControl(void){
  bool status = ConsoleWrite("AT&K0\r");

}

int sendSBDText(IridiumSBD* self,const char *message){

}
int sendSBDBinary(IridiumSBD* self,const uint8_t *txData, size_t txDataSize){

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
