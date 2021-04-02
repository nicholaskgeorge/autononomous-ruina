/*
 * File:   sdcard.h
 * This header file provides prototypes and definitions for the application
 * Author: Daisy Zhang
 *
 * Created on Dec 5, 2020, 3:38 PM
 */

#ifndef _SDCARD_H
#define _SDCARD_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************


#include "configuration.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <stdio.h>
#include <time.h>
#include "Helper_func.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

#define MAX_SD_CARD_READ_BUFFER_SIZE    200
#define SD_CARD_DEFAULT_TIMOUT   30

// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{

    SDCARD_STATE_CARD_MOUNT,
    SDCARD_STATE_CARD_CURRENT_DRIVE_SET,
    SDCARD_STATE_OPEN_WRITE_FILE,
    SDCARD_STATE_OPEN_READ_FILE,
    SDCARD_STATE_WRITE_FILE,
    SDCARD_STATE_READ_FILE,
    SDCARD_STATE_ERROR,
    SDCARD_STATE_WAITING,
    SDCARD_STATE_SUCCESS,
    SDCARD_STATE_READ_FILE_SIZE,
    SDCARD_STATE_CARD_READ,
    SDCARD_STATE_TIMEOUT


} SDCARD_STATES;


// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* The application's current state */
    SDCARD_STATES state;
    /* File Handle for tone.txt */
    SYS_FS_HANDLE fileHandle;
    /* Write (true) or read (false) a file*/
    bool WorR;
    /* Name of file(s) to be open*/
    char* fileOpen;
    /* File Size*/
    int32_t fileSize;
    /* Message to be written/read */
    uint8_t *buf;
    /* if message buffer is filled */
    bool bufFilled;
    /* size of buf*/
    size_t nbytes;
    /* size of buf after operation*/
    size_t nbytesReturned;
    /* Current file position or number of bytes read from file */
    int32_t currentFilePosition;
    /* Device name*/
    char* devName;
    /* Mounted drive name*/
    char* mntName;
//    /* Timer object */
//    SYS_TIME_HANDLE timeHandle;

} SDCARD_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void SDCARD_Initialize(void);

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    SDCARD_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void SDCARD_Initialize(SDCARD_DATA* SdcardData);

/*******************************************************************************
  Function:
    void SDCARD_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    SDCARD_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void SDCARD_Tasks( SDCARD_DATA* SdcardData );

/* Change the file operation if read or write to the file */
void SDCARD_WriteorRead(SDCARD_DATA* SdcardData,bool ifwrite);

/* Change the file to be written or read */
void SDCARD_FileName(SDCARD_DATA* SdcardData,char* fileName);

/* Fill in buffer to Write to the file */
void SDCARD_FillinBuffer(SDCARD_DATA* SdcardData,uint8_t* buf,size_t nbytes);

/* Switch sd card state only when it's waiting */
void SDCARD_StateSwitch(SDCARD_DATA* SdcardData,SDCARD_STATES state);

/* Change device for data logging */
void SDCARD_SetDevice(SDCARD_DATA* SdcardData,char* devName);

/* Change mount drive */
void SDCARD_SetMount(SDCARD_DATA* SdcardData,char* mntName);

#endif /* _APP_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */
