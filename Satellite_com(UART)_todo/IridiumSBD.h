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

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <stdio.h>

typedef enum{

  ISBD_SUCCESS          =   0,
  ISBD_ALREADY_AWAKE    =   1,
  ISBD_SERIAL_FAILURE   =   2,
  ISBD_PROTOCOL_ERROR    =  3,
  ISBD_CANCELLED          = 4,
  ISBD_NO_MODEM_DETECTED  = 5,
  ISBD_SBDIX_FATAL_ERROR  = 6,
  ISBD_SENDRECEIVE_TIMEOUT =7,
  ISBD_RX_OVERFLOW         =8,
  ISBD_REENTRANT           =9,
  ISBD_IS_ASLEEP           =10,
  ISBD_NO_SLEEP_PIN        =11,
  ISBD_NO_NETWORK          =12,
  ISBD_MSG_TOO_LONG        =13

}SBD_MODE;

typedef struct{

    PIO_PIN sleep_pin;
    PIO_PIN ring_pin;
    SBD_MODE current_mode;
    int atTimeout = 20;
    bool MOsent;
    int MOnum;
    bool MTreceive;
    int MTnum;
    int MTlen;
    int MTqueue;

} IridiumSBD;


bool disableFlowControl(void);
void adjustATTimeout(IridiumSBD* self,,int seconds);          // default value = 20 seconds
bool waitForATResponse(IridiumSBD* self,char *response=NULL, int responseSize=0, const char *prompt=NULL, const char *terminator="OK\r\n");
int sendSBDText(IridiumSBD* self,const char *message);
int sendSBDBinary(IridiumSBD* self,const uint8_t *txData, size_t txDataSize);
int sendReceiveSBDText(IridiumSBD* self,const char *message, uint8_t *rxBuffer, size_t &rxBufferSize);
int sendReceiveSBDBinary(IridiumSBD* self,const uint8_t *txData, size_t txDataSize, uint8_t *rxBuffer, size_t &rxBufferSize);
int checkMailBox(vIridiumSBD* self);
int getSignalQuality(IridiumSBD* self,int &quality);
int getSystemTime(IridiumSBD* self,struct tm &tm);
int sleep(IridiumSBD* self);
void power(IridiumSBD* self,bool on);
int begin(IridiumSBD* self);
#ifdef	__cplusplus
}
#endif

#endif	/* IRISIUMSBD_H */
