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
    SdcardData.state = SDCARD_STATE_CARD_MOUNT;
    SdcardData.currentFilePosition = 0;
    SdcardData.fileOpen = "";
    SdcardData.WorR = true;
    SdcardData.buf = "";
    SdcardData.nbytes = 0;
}

/* This is the task routine for this lab */

void SDCARD_Tasks(void)
{
    switch(SdcardData.state)
    {

            case SDCARD_STATE_CARD_MOUNT:
              if(SYS_FS_Mount("/dev/mmcblka1", "/mnt/myDrive", FAT, 0, NULL) != 0)
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
              if(SYS_FS_CurrentDriveSet("/mnt/myDrive") == SYS_FS_RES_FAILURE)
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
              SdcardData.fileHandle = SYS_FS_FileOpen(SdcardData.fileOpen, SYS_FS_FILE_OPEN_APPEND);
              if(SdcardData.fileHandle == SYS_FS_HANDLE_INVALID)
              {
                  /* Could not open the file. Error out*/
                  SdcardData.state = SDCARD_STATE_ERROR;
              }
              else
              {
                bytes_written = SYS_FS_FileWrite(SdcardData.fileHandle, (void *)SdcardData.buf, SdcardData.nbytes);
                if (bytes_written != -1){
                  SdcardData.state = SDCARD_STATE_WAITING;
                }else{
                  SdcardData.state = SDCARD_STATE_ERROR;
                }
                SYS_FS_FileClose(handle);
              }
              break;

            case SDCARD_STATE_WAITING:
              // do nothing until state changed
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

            case SDCARD_STATE_CARD_READ:

              nBytesRead = SYS_FS_FileRead(SdcardData.fileHandle,(void *)SdcardData.buf, SdcardData.nbytes);
              if (nBytesRead != -1){
                SdcardData.state = SDCARD_STATE_WAITING;
              }else{
                SdcardData.state = SDCARD_STATE_ERROR;
              }
              SYS_FS_FileClose(handle);

              break;



            default:
            {
            }
            break;
    }
}




/**********************************************************
 * Application CODEC buffer Event handler.
 ***********************************************************/

////////////////////////////////////////////////////////////////////////////////
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
