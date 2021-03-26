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

  power(true);
  SBDstatus.sleep_pin = SBD_SLEEP_PIN; //todo: set pin
  SBDstatus.ring_pin = SBD_RING_PIN;
  SBDstatus.current_mode = 	begin();
  SBDstatus.atTimeout = 20;
  SBDstatus.MOsent = false;
  SBDstatus.MOnum = 0;
  SBDstatus.MTreceive = false;
  SBDstatus.MTnum = 0;
  SBDstatus.MTlen = 0;
  SBDstatus.MTqueue = 0;

}

void adjustATTimeout(int seconds){
  SBDstatus.atTimeout = seconds;
}


bool waitForATResponse(char *response=NULL, int responseSize=0, const char *prompt=NULL, const char *terminator="OK\r\n"){
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
   int seconds = SBDstatus.atTimeout;

    endwait = time (NULL) + seconds ;
    while (time (NULL) < endwait){
     while (i<nbytes){

       char c = buf[i];
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
          if (terminator[matchTerminatorPos]== '\0' ){
             return true;
          }
       }else{
          matchTerminatorPos = c == terminator[0] ? 1:0;
       }

     }
     }
  return false;
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




int sleep(void){
  if (self->sleepPin == -1)
      return ISBD_NO_SLEEP_PIN;
  if (self->asleep)
      return ISBD_IS_ASLEEP;
  power(false);
  return ISBD_SUCCESS;
}

void power(bool on)
{
   SBDstatus.isAsleep = !on;

   if (SBDstatus.sleepPin == -1)
      return;

   PIO_PinOutputEnable(SBDstatus.sleep_pin);

   if (on)
   {
      debug_print("Powering on modem...\r\n");
      PIO_PinWrite(SBDstatus.sleep_pin, true);
      SBDstatus.lastPowerOnTime = time(NULL);
   }

   else
   {
      // Best Practices Guide suggests waiting at least 2 seconds
      // before powering off again
      double elapsed = difftime(time(NULL), SBDstatus.lastPowerOnTime);
      if (elapsed < 2)
         sleep(2);

      debug_print("Powering off modem...\r\n");

      PIO_PinWrite(SBDstatus.sleep_pin, false);
   }
}

int begin(void){
  time_t endwait;
  int seconds = SBDstatus.atTimeout;

   endwait = time (NULL) + seconds ;
   bool modemAlive = false;
   while (time (NULL) < endwait && !modemAlive){
     send("AT\r");
     modemAlive = waitForATResponse();
   }

   if (!modemAlive){
     debug_print("No modem detected.\r\n");
     return ISBD_NO_MODEM_DETECTED;
   }

   send("AT&K0\r");
   if (!waitForATResponse()){
     debug_print("Protocal error!\r\n");
     return ISBD_PROTOCOL_ERROR;
   }
   debug_print("InternalBegin: success!\r\n");
   return ISBD_SUCCESS;
}

/******************************************** HELPER FUNCTION ******************************************/

/* print message on debug port */
void debug_print(char* str){
  USART1_Write(str,sizeof(str));
}

/* Write to IridiumSBD */
bool send( void *buffer){
  bool status = UART0_Write(buffer, sizeof(buffer));
  return status;
}
