/*
 * File:   sdcard.c
 * This file contains the source code for the MPLAB Harmony application.
 * Author: Daisy Zhang
 *
 * Created on Dec 5, 2020, 3:38 PM
 */

#include "sdcard.h"


SDCARD_DATA SdcardData;

/* This is initialization function */

void SDCARD_Initialize(void)
{
    SdcardData.state = SDCARD_STATE_WAITING;
    SdcardData.currentFilePosition = 0;
    SdcardData.fileOpen = "";
    SdcardData.WorR = true; // true -> write to file; false -> read file
    SdcardData.buf = 0;
    SdcardData.nbytes = 128;
    SdcardData.nbytesReturned = 0;
    SdcardData.devName = "/dev/mmcblka1";
    SdcardData.mntName = "/mnt/myDrive";
}

/* This is the task routine for this lab */

void SDCARD_Tasks(void)
{
    switch(SdcardData.state)
    {

            case SDCARD_STATE_CARD_MOUNT:
              if(SYS_FS_Mount(SdcardData.devName, SdcardData.mntName, FAT, 0, NULL) != SYS_FS_RES_SUCCESS)
              {
                  /* The disk could not be mounted. Try mounting again until success. */
                  SdcardData.state = SDCARD_STATE_CARD_MOUNT;


              }
              else
              {
                  /* Mount was successful. Unmount the disk, for testing. */
                  SdcardData.state = SDCARD_STATE_CARD_CURRENT_DRIVE_SET;
              }
              break;


            case SDCARD_STATE_CARD_CURRENT_DRIVE_SET:
              if(SYS_FS_CurrentDriveSet(SdcardData.mntName) == SYS_FS_RES_FAILURE)
              {
                  /* Error while setting current drive */
                  SdcardData.state = SDCARD_STATE_ERROR;
              }
              else
              {

                if(SdcardData.WorR){
                  SdcardData.state = SDCARD_STATE_WRITE_FILE;
                }else{
                  SdcardData.state = SDCARD_STATE_READ_FILE;

                }
              }
              break;

            case SDCARD_STATE_WRITE_FILE:
              SdcardData.fileHandle = SYS_FS_FileOpen(SdcardData.fileOpen, SYS_FS_FILE_OPEN_APPEND);// try create a new file named "fileopen" and see
              if(SdcardData.fileHandle == SYS_FS_HANDLE_INVALID)
              {
                  /* Could not open the file. Error out*/
                  SdcardData.state = SDCARD_STATE_ERROR;
              }
              else
              {
                SdcardData.nbytesReturned = SYS_FS_FileWrite(SdcardData.fileHandle, (void *)SdcardData.buf, SdcardData.nbytes);
                if (SdcardData.nbytesReturned != -1){
                  SdcardData.state = SDCARD_STATE_SUCCESS;
                }else{
                  SdcardData.state = SDCARD_STATE_ERROR;
                }
                SYS_FS_FileClose(SdcardData.fileHandle);
              }
              break;

            case SDCARD_STATE_READ_FILE:
              SdcardData.fileHandle = SYS_FS_FileOpen(SdcardData.fileOpen, SYS_FS_FILE_OPEN_READ);
              if(SdcardData.fileHandle == SYS_FS_HANDLE_INVALID)
              {
                  /* Could not open the file. Error out*/
                  SdcardData.state = SDCARD_STATE_ERROR;
              }
              else{
                  /* Read the file size */
                  SdcardData.state = SDCARD_STATE_READ_FILE_SIZE;
              }

              break;

            case SDCARD_STATE_READ_FILE_SIZE:
              SdcardData.fileSize = SYS_FS_FileSize( SdcardData.fileHandle);

              if (SdcardData.fileSize == -1)
              {
                  /* Could not read file size. Error out*/
                  SdcardData.state = SDCARD_STATE_ERROR;
              }
              else
              {
                  SdcardData.state = SDCARD_STATE_CARD_READ;
              }

              break;

            case SDCARD_STATE_CARD_READ:;

              SdcardData.nbytesReturned = SYS_FS_FileRead(SdcardData.fileHandle,(void *)SdcardData.buf, SdcardData.nbytes);
              if (SdcardData.nbytesReturned != -1){
                SdcardData.state = SDCARD_STATE_SUCCESS;
              }else{
                SdcardData.state = SDCARD_STATE_ERROR;
              }
              SYS_FS_FileClose(SdcardData.fileHandle);
              break;

            case SDCARD_STATE_ERROR:;
              SYS_FS_ERROR err = SYS_FS_Error();
              char fail_message[50];
              sprintf(fail_message, "FAIL MESSAGE:%i\r\n", err);
              USART1_Write(fail_message,sizeof(fail_message));
              break;

            case SDCARD_STATE_SUCCESS:
              USART1_Write("File Operation Success!\r\n",sizeof("File Operation Success!\r\n"));
              SdcardData.state = SDCARD_STATE_WAITING;
              break;

            case SDCARD_STATE_WAITING:
              // do nothing until state changed
              break;

            default:
            {
            }
            break;
    }
}

void SDCARD_WriteorRead(bool ifwrite){
  SdcardData.WorR = ifwrite;
}

void SDCARD_FileName(char* fileName){
  SdcardData.fileOpen = fileName;
}

void SDCARD_StateSwitch(SDCARD_STATES state){
  if (SdcardData.state == SDCARD_STATE_WAITING)
    SdcardData.state = state;
}

void SDCARD_FillBuffer(uint8_t* buf,size_t nbytes){
  SdcardData.buf = buf;
  SdcardData.nbytes = nbytes;
}

void SDCARD_SetDevice(char* devName){
  SdcardData.devName = devName;
}

void SDCARD_SetMount(char* mntName){
  SdcardData.mntName = mntName;
}

/* Close file
* Input:
*        handle: file handle
* Returns: if file is successfully closed
*/
 bool SDCARD_CloseFile (SYS_FS_HANDLE fileHandle )
{
    SYS_FS_RESULT ret;
    if ( fileHandle != SYS_FS_HANDLE_INVALID )
    {
        ret = SYS_FS_FileClose ( fileHandle );
        if(ret == SYS_FS_RES_SUCCESS)
        {
            return true;
        }
    }
    return false;
}
