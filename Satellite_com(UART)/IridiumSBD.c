/*
 * File:  IridiumSBD.c
 * Satellite communication library
 * Author: Daisy Zhang
 *
 * Created on Mar, 26, 2021, 3:38 PM
 */

#include "IridiumSBD.h"

/*========================== HELPER FUNCTION STARTS =============================*/

/* print message on debug port */
void debug_print(char* str){
  USART1_Write(str,sizeof(str));
}

void debug_print_int(uint16_t num){
   char* p = malloc(sizeof(char) * (ISBD_MAX_MESSAGE_LENGTH + 1));;
   sprintf(p,"%u",num);
   debug_print(p);
   free(p);
}

void debug_print_size(size_t num){
   char* p= malloc(sizeof(char) * (ISBD_MAX_MESSAGE_LENGTH + 1));
   sprintf(p,"%zu",num);
   debug_print(p);
   free(p);
}

/* Write to IridiumSBD */
bool send(char *buffer){
   debug_print(">> ");
   debug_print(buffer);
   debug_print("\r\n");
   bool status = UART0_Write((void*)buffer, sizeof((void*)buffer));
   return status;
}

bool send_int(uint16_t num){
   char* p= malloc(sizeof(char) * (ISBD_MAX_MESSAGE_LENGTH + 1));
   sprintf(p,"%u",num);
   bool status =send(p);
   free(p);
   return status;
}

bool send_size(size_t num){
   char* p= malloc(sizeof(char) * (ISBD_MAX_MESSAGE_LENGTH + 1));
   sprintf(p,"%zu",num);
   bool status = send(p);
   free(p);
   return status;
}

/* parse to read SBDIX response field */
int doSBDIX(IridiumSBD* self,uint16_t *moCode, uint16_t *moMSN, uint16_t *mtCode, uint16_t *mtMSN, uint16_t *mtLen, uint16_t *mtRemaining){
   // Returns xx,xxxxx,xx,xxxxx,xx,xxx
   char sbdixResponseBuf[32];
   send("AT+SBDIX\r");
   if (!waitForATResponse(self,sbdixResponseBuf, sizeof(sbdixResponseBuf),NULL, "+SBDIX: "))
      return ISBD_PROTOCOL_ERROR;

   uint16_t *values[6] = { moCode, moMSN, mtCode, mtMSN, mtLen, mtRemaining };
   for (int i=0; i<6; ++i)
   {
      char *p = strtok(i == 0 ? sbdixResponseBuf : NULL, ", ");
      if (p == NULL)
         return ISBD_PROTOCOL_ERROR;
      *values[i] = atol(p);
   }
   return ISBD_SUCCESS;
}

/* Receive messages in ASCII */
int doSBDRT(IridiumSBD* self,char *rxBuffer, size_t *rxBufferSize){
   send("AT+SBDRT\r");
   char sbdrtResponseBuf[ISBD_MAX_MESSAGE_LENGTH];

   if (!waitForATResponse(self,sbdrtResponseBuf, sizeof(sbdrtResponseBuf),NULL, "+SBDRT:\r\n"))
      return ISBD_PROTOCOL_ERROR;

   char* p = strtok(sbdrtResponseBuf,"\r");
   rxBuffer = p;
   *rxBufferSize = sizeof(p);
   return ISBD_SUCCESS;
}

/*========================== HELPER FUNCTION ENDS =============================*/


void SBD_Initialize(IridiumSBD* self){

  self->sleep_pin = -1; // or, SBD_SLEEP_PIN; //todo: set pin
  self->ring_pin = -1;
  self->atTimeout = ISBD_DEFAULT_AT_TIMEOUT;
  self->sendReceiveTimeout = ISBD_DEFAULT_SENDRECEIVE_TIME;
  power(self,true);

  begin(self);
  if (self->current_mode != ISBD_SUCCESS){
     power(self,false);
  }
  self->MOsent = false;
  self->MOnum = 0;
  self->MTreceive = false;
  self->MTnum = 0;
  self->MTlen = 0;
  self->MTqueue = 0;

}

/* TODO */ /* TODO */ /* TODO */ /* TODO */ /* TODO */ /* TODO */ /* TODO */
void SBD_Tasks(IridiumSBD* self){
   return;
   // adaptive retry
}
 /* TODO */ /* TODO */ /* TODO */ /* TODO */ /* TODO */ /* TODO */ /* TODO */ /* TODO */



void adjustATTimeout(IridiumSBD* self,int seconds){
  self->atTimeout = seconds;
}


bool waitForATResponse(IridiumSBD* self,char *response, int responseSize, const char *prompt, const char *terminator){
   if (response)
      memset(response,0,responseSize);

   int matchPromptPos = 0; // Matched chars in prompt
   int matchTerminatorPos = 0; // Matched chars in terminator
   enum {LOOKING_FOR_PROMPT, GATHERING_RESPONSE, LOOKING_FOR_TERMINATOR};
   int promptState = prompt ? LOOKING_FOR_PROMPT : LOOKING_FOR_TERMINATOR;

   uint8_t buf[128];
   size_t nbytes = 128;

   USART0_Read(&buf[0], nbytes);

   int i = 0;

   time_t endwait;
   int seconds = self->atTimeout;

   endwait = time (NULL) + seconds ;
   while (time (NULL) < endwait){
      while (i<nbytes){

       char c = buf[i++];
       if (prompt){
          switch (promptState){
             case LOOKING_FOR_PROMPT:
                if (c == prompt[matchPromptPos]){
                   ++matchPromptPos;
                   if (prompt[matchPromptPos] == '\0')
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
          if (terminator[matchTerminatorPos]== '\0' ){ // check echo strings
             return true;
          }
       }else{
          matchTerminatorPos = c == terminator[0] ? 1:0;
       }

      }
     }
  return false;
}


void sendSBDText(IridiumSBD* self,char *message){

   debug_print("Send text msg...\r\n");

   if (message == NULL){
      send("AT+SBDWT=\r");
      if (!waitForATResponse(self,NULL,0,NULL,"OK\r\n")){
         self->current_mode= ISBD_PROTOCOL_ERROR;
         return;
      }
   }
   else{
      char *p = strchr(message, '\r'); // remove any embedded \r

      if (strlen(p)> ISBD_MAX_MESSAGE_LENGTH){
         self->current_mode = ISBD_MSG_TOO_LONG;
         return;
      }
      send("AT+SBDWT\r");
      if (!waitForATResponse(self,NULL, 0, NULL, "READY\r\n")){
         self->current_mode = ISBD_PROTOCOL_ERROR;
         return;
      }
      send(p);
      send("\r");
      if (!waitForATResponse(self,NULL, 0, NULL, "0\r\n\r\nOK\r\n")){
         self->current_mode = ISBD_PROTOCOL_ERROR;
         return;
      }
   }
   self->current_mode = ISBD_SUCCESS;
   debug_print("Done!\r\n");

}

void sendSBDBinary(IridiumSBD* self,uint8_t *txData, size_t txDataSize){
   debug_print("Send binary msg...\r\n");

   if (txDataSize > ISBD_MAX_MESSAGE_LENGTH){
      self->current_mode = ISBD_MSG_TOO_LONG;
      return;
   }

   char* p = malloc(sizeof(char) * (ISBD_MAX_MESSAGE_LENGTH + 1));
   sprintf(p,"AT+SBDWD=%zu\r",txDataSize);
   send(p);
   free(p);
   if (!waitForATResponse(self,NULL, 0, NULL, "READY\r\n")){
      self->current_mode = ISBD_PROTOCOL_ERROR;
      return;
   }

   uint16_t checksum = 0;
   for (size_t i=0; i<txDataSize; ++i)
   {
      send((char *)&txData[i]);
      checksum += (uint16_t)txData[i];
   }
   send_int(checksum >> 8);
   send_int(checksum & 0xFF);

   if (!waitForATResponse(self,NULL, 0, NULL, "0\r\n\r\nOK\r\n")){
      self->current_mode = ISBD_PROTOCOL_ERROR;
      return;
   }

   self->current_mode = ISBD_SUCCESS;


   debug_print("Done!\r\n");
}


void checkMailBox(IridiumSBD* self,char *rxBuffer, size_t *rxBufferSize){
   // Long SBDIX loop begins here

   time_t endwait;
   int seconds = self->sendReceiveTimeout;
   endwait = time (NULL) + seconds ;
   while (time (NULL) < endwait){

      uint16_t moCode = 0, moMSN = 0, mtCode = 0, mtMSN = 0, mtLen = 0, mtRemaining = 0;
      int ret = doSBDIX(self,&moCode, &moMSN, &mtCode, &mtMSN, &mtLen, &mtRemaining);

      if (ret != ISBD_SUCCESS){
         self->current_mode = ret;
         return;
      }

      debug_print("SBDIX MO code: ");
      debug_print_int(moCode);
      debug_print("\r\n");

      if (moCode >= 0 && moCode <= 4) // this range indicates successful return!
      {
         debug_print("SBDIX success!\r\n");
         self->MOsent = true;
         self->MOnum = moMSN;
         self->MTnum = mtMSN;
         self->MTlen = mtLen;
         self->MTqueue = mtRemaining;

         self->MTreceive = mtCode == 1 && rxBuffer;
         if (self->MTreceive) // retrieved 1 message
         {
            debug_print("Incoming message!\r\n");
            self->current_mode = doSBDRT(self,rxBuffer, rxBufferSize);
            return;
         }

         else
         {
            // No data returned
            if (rxBufferSize)
               *rxBufferSize = 0;
         }
         self->current_mode = ISBD_SUCCESS;
         return;
      }

      else if (moCode == 12 || moCode == 14 || moCode == 16) // fatal failure: no retry
      {
         debug_print("SBDIX fatal!\r\n");
         self->current_mode = ISBD_SBDIX_FATAL_ERROR;
         return;
      }

      else // retry
      {
         debug_print("Waiting for SBDIX retry...\r\n");
         self->current_mode = ISBD_WAIT_RETRY;
         return;
      }

   } // big wait loop

   debug_print("SBDIX timeout!\r\n");
   self->current_mode = ISBD_SENDRECEIVE_TIMEOUT;
   return;
}


void getSignalQuality(IridiumSBD* self,int* quality){
  if (self->isAsleep){
    self->current_mode = ISBD_IS_ASLEEP;
    return;
  }
  char csqResponseBuf[2];

  send("AT+CSQ\r");
  if (!waitForATResponse(self,csqResponseBuf, sizeof(csqResponseBuf),NULL, "+CSQ:")){
      self->current_mode = ISBD_PROTOCOL_ERROR;
      return;
   }

    if (isdigit(csqResponseBuf[0]))
    {
      *quality = atoi(csqResponseBuf);
      self->current_mode = ISBD_SUCCESS;
      return;
    }

    self->current_mode = ISBD_PROTOCOL_ERROR;
}

void sleep_ISBD(IridiumSBD* self){
  if (self->sleep_pin == -1)
      self->current_mode  =  ISBD_NO_SLEEP_PIN;
  if (self->isAsleep)
      self->current_mode  =  ISBD_IS_ASLEEP;
  power(self,false);
  self->current_mode  =  ISBD_SUCCESS;
}

void power(IridiumSBD* self,bool on)
{
   self->isAsleep = !on;

   if (self->sleep_pin == -1)
      return;

   PIO_PinOutputEnable(self->sleep_pin);

   if (on)
   {
      debug_print("Powering on modem...\r\n");
      PIO_PinWrite(self->sleep_pin, true);
      self->lastPowerOnTime = time(NULL);
   }

   else
   {
      // Best Practices Guide suggests waiting at least 2 seconds
      // before powering off again
      double elapsed = difftime(time(NULL), self->lastPowerOnTime);
      if (elapsed < 2)
         sleep(2);

      debug_print("Powering off modem...\r\n");

      PIO_PinWrite(self->sleep_pin, false);
   }
}

void begin(IridiumSBD* self){
  time_t endwait;
  int seconds = self->atTimeout;

   endwait = time (NULL) + seconds ;
   bool modemAlive = false;
   while (time (NULL) < endwait && !modemAlive){
     send("AT\r");
     modemAlive = waitForATResponse(self,NULL,0,NULL,"OK\r\n");
   }

   if (!modemAlive){
     debug_print("No modem detected.\r\n");
     self->current_mode  = ISBD_NO_MODEM_DETECTED;
   }

   send("AT&K0\r");
   if (!waitForATResponse(self,NULL,0,NULL,"OK\r\n")){
     debug_print("Protocal error!\r\n");
     self->current_mode  =  ISBD_PROTOCOL_ERROR;
   }
   debug_print("InternalBegin: success!\r\n");
   self->current_mode  =  ISBD_SUCCESS;
}
