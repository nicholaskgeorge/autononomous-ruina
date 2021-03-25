/*
 * File:  IridiumSBD.c
 * Satellite communication library
 * Author: Daisy Zhang
 *
 * Created on August 26, 2020, 3:38 PM
 */

#include "IridiumSBD.h"


IridiumSBD SBDstatus;

void SBD_Initialize(void){

  SBDstatus.sleep_pin;
  SBDstatus.ring_pin;
  SBDstatus.current_mode;
  SBDstatus.atTimeout = 20;
  SBDstatus.MOsent;
  SBDstatus.MOnum;
  SBDstatus.MTreceive;
  SBDstatus.MTnum;
  SBDstatus.MTlen;
  SBDstatus.MTqueue;



}


/* Write to IridiumSBD
 * Input: buffer - contents to write
 * Returns: if the operation succeeds
 */
bool send( void *buffer){
  bool status = UART0_Write(buffer, sizeof(buffer));
  return status;
}

void adjustATTimeout(int seconds){
  SBDstatus.atTimeout = seconds;
}

/*
Wait for response from previous AT command.
This process terminates when "terminator" string is seen or upon timeout.
If "prompt" string is provided (example "+CSQ:"), then all characters following
prompt up to the next CRLF are stored in response buffer for later parsing by caller.
 */
bool waitForATResponse(IridiumSBD* self,char *response=NULL, int responseSize=0, const char *prompt=NULL, const char *terminator="OK\r\n"){
   if (response)
      memset(response,0,responseSize);

   int matchPromptPos = 0; // Matched chars in prompt
   int matchTerminatorPos = 0; // Matched chars in terminator
   enum {LOOKING_FOR_PROMPT, GATHERING_RESPONSE, LOOKING_FOR_TERMINATOR};
   int promptState = prompt ? LOOKING_FOR_PROMPT : LOOKING_FOR_TERMINATOR;

   // TODO: set a timer /////////////////////////////////////////////////////////////////////////////////
   // while the timer is not expired:////////////////////////////////////////////////////////////////////
   char* incoming_str = UART0_Read(); // read full message //////////////////////////////////////////////
   // while there's char available://////////////////////////////////////////////////////////////////////
   char c = incoming_str;
   if (prompt){
      switch (promptState){
         case LOOKING_FOR_PROMPT:
            if (c == prompt[matchPromptPos]){
               ++matchPromptPos;
               if (promp[matchPromptPos] == '\0')
                  promptState = GATHERING_RESPONSE;
            }else{
               matchPromptPos = c == prompt[0] ? 1 : 0;
            }
            break;
         case GATHERING_RESPONSE:
            if (response){
               if (c == '\r' || responseSize <2){
                  promptState = LOOKING_FOR_TERMINATOR;
               }else{
                  *response++ = c;
                  responseSize--;
               }
            }
            break;
      }
   }

   if (c == terminator[matchTerminatorPos]){
      ++matchTerminatorPos;
      if (terminator[matchTerminatorPos]== '\0' ){
         return true;
      }
   }else{
      matchTerminatorPos = c == terminator[0] ? 1:0;
   }
   // end of while ava. loop
   // end of timer loop
}


bool disableFlowControl(void){
  bool status = send("AT&K0\r");
  return status
}

/* TODO */
int sendSBDText(IridiumSBD* self,const char *message){

}

/* TODO */
int sendSBDBinary(IridiumSBD* self,const uint8_t *txData, size_t txDataSize){

  send("AT+SBDWB=[");
  send(txDataSize);// todo : int to string
  send("]\r");

  if (!waitForATResponse(NULL, 0, NULL, "READY\r\n"))
         return cancelled() ? ISBD_CANCELLED : ISBD_PROTOCOL_ERROR; /////////////////////////////////////////////////////////// canceled todo

  // send txData + checksum
  send(txTxtMessage);
  send("\r");
  // wait for 0\r\n\r\nOK\r\n
  if (!waitForATResponse(NULL, 0, NULL, "0\r\n\r\nOK\r\n"))
         return cancelled() ? ISBD_CANCELLED : ISBD_PROTOCOL_ERROR; /////////////////////////////////////////////////////////// canceled todo
  }
}

/* TODO */
int sendReceiveSBDText(IridiumSBD* self,const char *message, uint8_t *rxBuffer, size_t &rxBufferSize){

  send("AT+SBDWT=");
  send(message);
  send("\r")
  // wait for OK\r

}

/* TODO */
int sendReceiveSBDBinary(IridiumSBD* self,const uint8_t *txData, size_t txDataSize, uint8_t *rxBuffer, size_t &rxBufferSize){
  send("AT+SBDRT\r");
  // wait message

}

/* TODO */
int checkMailBox(IridiumSBD* self){
  send("AT+SBDIX\r");
  // receive response +SBDIX:
}

/* TODO */
int getSignalQuality(IridiumSBD* self,int &quality){
  if (self->asleep)
    return ISBD_IS_ASLEEP;

  char csqResponseBuf[2];

  send("AT+CSQ\r");
  if (!waitForATResponse(csqResponseBuf, sizeof(csqResponseBuf), "+CSQ:"))
      return cancelled() ? ISBD_CANCELLED : ISBD_PROTOCOL_ERROR;

    if (isdigit(csqResponseBuf[0]))
    {
      quality = atoi(csqResponseBuf);
      return ISBD_SUCCESS;
    }

    return ISBD_PROTOCOL_ERROR;
}

/* TODO */
int getSystemTime(IridiumSBD* self,struct tm &tm){
   char msstmResponseBuf[24];

   send(F("AT-MSSTM\r"));
   if (!waitForATResponse(msstmResponseBuf, sizeof(msstmResponseBuf), "-MSSTM: "))
      return cancelled() ? ISBD_CANCELLED : ISBD_PROTOCOL_ERROR;

   if (!isxdigit(msstmResponseBuf[0]))
      return ISBD_NO_NETWORK;

   // Latest epoch began at May 11, 2014, at 14:23:55 UTC.
   struct tm epoch_start;
   epoch_start.tm_year = 2014 - 1900;
   epoch_start.tm_mon = 5 - 1;
   epoch_start.tm_mday = 11;
   epoch_start.tm_hour = 14;
   epoch_start.tm_min = 23;
   epoch_start.tm_sec = 55;

   unsigned long ticks_since_epoch = strtoul(msstmResponseBuf, NULL, 16);

   /* Strategy: we'll convert to seconds by finding the largest number of integral
      seconds less than the equivalent ticks_since_epoch. Subtract that away and
      we'll be left with a small number that won't overflow when we scale by 90/1000.
      Many thanks to Scott Weldon for this suggestion.
   */
   unsigned long secs_since_epoch = (ticks_since_epoch / 1000) * 90;
   unsigned long small_ticks = ticks_since_epoch - (secs_since_epoch / 90) * 1000;
   secs_since_epoch += small_ticks * 90 / 1000;

   time_t epoch_time = mktime(&epoch_start);
   time_t now = epoch_time + secs_since_epoch;
   memcpy(&tm, localtime(&now), sizeof tm);
   return ISBD_SUCCESS;

}

/* TODO */
int sleep(IridiumSBD* self){
  if (self->sleepPin == -1)
      return ISBD_NO_SLEEP_PIN;
  if (self->asleep)
      return ISBD_IS_ASLEEP;
  power(self,false);
  self->asleep
  return ISBD_SUCCESS;
}

/* TODO */
int begin(IridiumSBD* self){
   if (!self->asleep)
      return ISBD_ALREADY_AWAKE;

   power(true); // power on

   bool modemAlive = false;

   unsigned long startupTime = 500; //ms
   for (unsigned long start = millis(); millis() - start < startupTime;)
      if (cancelled())
         return ISBD_CANCELLED;

   // Turn on modem and wait for a response from "AT" command to begin
   for (unsigned long start = millis(); !modemAlive && millis() - start < 1000UL * ISBD_STARTUP_MAX_TIME;)
   {
      send(F("AT\r"));
      modemAlive = waitForATResponse();
      if (cancelled())
         return ISBD_CANCELLED;
   }

   if (!modemAlive)
   {
      diagprint(F("No modem detected.\r\n"));
      return ISBD_NO_MODEM_DETECTED;
   }

   // The usual initialization sequence
   const char *strings[3] = { "ATE1\r", "AT&D0\r", "AT&K0\r" };
   for (int i=0; i<3; ++i)
   {
      send(strings[i]);
      if (!waitForATResponse())
         return cancelled() ? ISBD_CANCELLED : ISBD_PROTOCOL_ERROR;
   }
}

/* TODO */
void power(IridiumSBD* self,bool on)
{
   self->asleep = !on;

   if (self->sleepPin == -1)
      return;
   // todo!!
   pinMode(self->sleepPin, OUTPUT);

   if (on)
   {
      diagprint(F("Powering on modem...\r\n"));
      // TODO!!!
      digitalWrite(self->sleepPin, HIGH); // HIGH = awake
      lastPowerOnTime = millis();
   }

   else
   {
      // Best Practices Guide suggests waiting at least 2 seconds
      // before powering off again
      unsigned long elapsed = millis() - lastPowerOnTime;
      if (elapsed < 2000UL)
         delay(2000UL - elapsed);

      diagprint(F("Powering off modem...\r\n"));
      //todo!!!
      digitalWrite(self->sleepPin, LOW); // LOW = asleep
   }
}
