/*
 * File:   IridiumSBD.h
 * Satellite communication library
 * Author: Daisy Zhang
 *
 * Created on August 27, 2020, 8:21 PM
 */

#ifndef IRIDIUMSBD_H
	IRIDIUMSBD_H

#ifdef	__cplusplus
extern "C" {
#endif


#include "configuration.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <stdio.h>
#include <math.h>
#include <string.h>
#include  <ctype.h>
#include <time.h>
#include <unistd.h>

#define ISBD_MAX_MESSAGE_LENGTH         340
#define ISBD_DEFAULT_AT_TIMEOUT         30
#define ISBD_DEFAULT_SENDRECEIVE_TIME   300

typedef enum{
  ISBD_SUCCESS          ,
  ISBD_ALREADY_AWAKE    ,
  ISBD_SERIAL_FAILURE   ,
  ISBD_PROTOCOL_ERROR    ,
  ISBD_NO_MODEM_DETECTED  ,
  ISBD_SBDIX_FATAL_ERROR  ,
  ISBD_SENDRECEIVE_TIMEOUT ,
  ISBD_RX_OVERFLOW         ,
  ISBD_REENTRANT           ,
  ISBD_IS_ASLEEP           ,
  ISBD_NO_SLEEP_PIN        ,
  ISBD_NO_NETWORK          ,
  ISBD_MSG_TOO_LONG,
  ISBD_WAIT_RETRY

}SBD_MODE;

typedef struct{

    PIO_PIN sleep_pin;
    PIO_PIN ring_pin;
    SBD_MODE current_mode;
    int atTimeout = ISBD_DEFAULT_AT_TIMEOUT;
    int sendReceiveTimeout = ISBD_DEFAULT_SENDRECEIVE_TIME;
    bool MOsent;
    uint16_t MOnum;
    bool MTreceive;
    uint16_t MTnum;
    uint16_t MTlen;
    uint16_t MTqueue;
		bool isAsleep;
		time_t lastPowerOnTime;

} IridiumSBD;

/*
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_Tasks function can be called.

*/
void SBD_Initialize(IridiumSBD* self);

/*
    This routine is the application's tasks function.  It
    defines the application's state machine and core logic.
 */
void SBD_Tasks(IridiumSBD* self); /* TODO */

/* Adjust time out in seconds. Defalult value is 20 seconds. */
void adjustATTimeout(IridiumSBD* self,int seconds);

/*
Wait for response from previous AT command.
This process terminates when "terminator" string is seen or upon timeout.
If "prompt" string is provided (example "+CSQ:"), then all characters following
prompt up to the next CRLF are stored in response buffer for later parsing by caller.
 */
bool waitForATResponse(IridiumSBD* self,char *response=NULL, int responseSize=0, const char *prompt=NULL, const char *terminator="OK\r\n");

/* send data in text */
void sendSBDText(IridiumSBD* self,const char *message);

/* send data in binary*/
void sendSBDBinary(IridiumSBD* self,const uint8_t *txData, size_t txDataSize);

/* mailbox check if any new message coming or if old message sent */
void checkMailBox(IridiumSBD* self,uint8_t *rxBuffer, size_t *rxBufferSize);

/* Get satellite signal quality */
void getSignalQuality(IridiumSBD* self,int &quality);

/* make the modem asleeep */
void sleep(IridiumSBD* self);

/* power on/off the modem */
void power(IridiumSBD* self,bool on);

/* The usual initialization process */
void begin(IridiumSBD* self);
#ifdef	__cplusplus
}
#endif

#endif	/* IRISIUMSBD_H */
