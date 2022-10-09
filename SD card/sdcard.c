/*
 * File:   sdcard.c
 * This file contains the source code for the MPLAB Harmony application.
 * Author: Daisy Zhang
 *
 * Created on Dec 5, 2020, 3:38 PM
 */

#include "sdcard.h"


//void SDMountTimeout_Callback (uintptr_t context)
//{
//
//}

/* This is initialization function */

void SDCARD_Initialize(SDCARD_DATA* SdcardData){
    SdcardData->state = SDCARD_STATE_WAITING;
    SdcardData->currentFilePosition = 0;
    SdcardData->fileOpen = "test.txt";
    SdcardData->WorR = true; // true -> write to file; false -> read file
    SdcardData->nbytesReturned = 0;
    SdcardData->devName = "/dev/ mmcblka1";
    SdcardData->mntName = "/mnt/myDrive1";
    SdcardData->bufFilled = false;
    SdcardData->buf = malloc(sizeof(uint8_t)*500); 
//    SdcardData->timeHandle = SYS_TIME_CallbackRegisterMS(SDMountTimeout_Callback, (uintptr_t)0, SD_CARD_DEFAULT_TIMOUT*1000, SYS_TIME_SINGLE);
}

/* This is the task routine for this lab */

void SDCARD_Tasks(SDCARD_DATA* SdcardData){
        switch(SdcardData->state)
        {

            case SDCARD_STATE_CARD_MOUNT:
                
              if(SYS_FS_Mount(SdcardData->devName ,SdcardData->mntName, FAT, 0, NULL) != SYS_FS_RES_SUCCESS)
              {
                  /* The disk could not be mounted. Try mounting again until success. */
                  asm("BKPT");
                  USART1_Write("Keep mounting...\r\n",sizeof("Keep mounting...\r\n"));
              }
              else
              {
                  /* Mount was successful. Unmount the disk, for testing. */
                  asm("BKPT");
                  SdcardData->state = SDCARD_STATE_CARD_CURRENT_DRIVE_SET;
              }
              break;


            case SDCARD_STATE_CARD_CURRENT_DRIVE_SET:

              if(SYS_FS_CurrentDriveSet(SdcardData->mntName) == SYS_FS_RES_FAILURE)
              {
                  /* Error while setting current drive */
                  asm("BKPT");
                  SdcardData->state = SDCARD_STATE_ERROR;
              }
              else
              {
                asm("BKPT");
                USART1_Write("Set Drive...\r\n",sizeof("Set Drive...\r\n"));
                if(SdcardData->WorR){
                  SdcardData->state = SDCARD_STATE_OPEN_WRITE_FILE;
                }else{
                  SdcardData->state = SDCARD_STATE_OPEN_READ_FILE;

                }
              }
              break;

            case SDCARD_STATE_OPEN_WRITE_FILE:
                //USART1_Write("Open file...\r\n",sizeof("Open file...\r\n"));
              SdcardData->fileHandle = SYS_FS_FileOpen(SdcardData->fileOpen, SYS_FS_FILE_OPEN_APPEND);// try create a new file named "fileopen" and see
              if(SdcardData->fileHandle == SYS_FS_HANDLE_INVALID)
              {
                  /* Could not open the file. Error out*/
                  SdcardData->state = SDCARD_STATE_ERROR;
              }
              else
              {
                SdcardData->state = SDCARD_STATE_WRITE_FILE;
              }
              break;

            case SDCARD_STATE_WRITE_FILE:


                if(!SdcardData->bufFilled){
                    SdcardData->state = SDCARD_STATE_WRITE_FILE;
                }else{
                    //USART1_Write(SdcardData->buf,SdcardData->nbytes);
                    SdcardData->nbytesReturned = SYS_FS_FileWrite(SdcardData->fileHandle,SdcardData->buf, SdcardData->nbytes);
                    if (SdcardData->nbytesReturned == -1){
                      SdcardData->state = SDCARD_STATE_ERROR;
                    }else{
                      SdcardData->state = SDCARD_STATE_SUCCESS;
                    }
                    SYS_FS_FileClose(SdcardData->fileHandle);
                    //USART1_Write("Write to file...\r\n",sizeof("Write to file...\r\n"));
                }


              break;

            case SDCARD_STATE_OPEN_READ_FILE:
              SdcardData->fileHandle = SYS_FS_FileOpen(SdcardData->fileOpen, SYS_FS_FILE_OPEN_READ);
              if(SdcardData->fileHandle == SYS_FS_HANDLE_INVALID)
              {
                  /* Could not open the file. Error out*/
                  SdcardData->state = SDCARD_STATE_ERROR;
              }
              else{
                  /* Read the file size */
                  SdcardData->state = SDCARD_STATE_READ_FILE_SIZE;
              }

              break;

            case SDCARD_STATE_READ_FILE_SIZE:
              SdcardData->fileSize = SYS_FS_FileSize( SdcardData->fileHandle);

              if (SdcardData->fileSize == -1)
              {
                  /* Could not read file size. Error out*/
                  SdcardData->state = SDCARD_STATE_ERROR;
              }
              else
              {
                  SdcardData->state = SDCARD_STATE_READ_FILE;
              }

              break;

            case SDCARD_STATE_READ_FILE:;

              SdcardData->nbytesReturned = SYS_FS_FileRead(SdcardData->fileHandle,(void *)SdcardData->buf, SdcardData->nbytes);

              if (SdcardData->nbytesReturned == -1  ){
                SdcardData->state = SDCARD_STATE_ERROR;
              }else{
                SdcardData->state = SDCARD_STATE_SUCCESS;
              }
              SYS_FS_FileClose(SdcardData->fileHandle);
              break;

            case SDCARD_STATE_ERROR:;
              SYS_FS_ERROR err = SYS_FS_Error();
              char fail_message[50];
              sprintf(fail_message, "FAIL MESSAGE:%i\r\n", err);
              USART1_Write(fail_message,sizeof(fail_message));
              break;

            case SDCARD_STATE_SUCCESS:
              USART1_Write("File Operation Success!\r\n",sizeof("File Operation Success!\r\n"));
              SdcardData->state = SDCARD_STATE_WAITING;
              break;

            case SDCARD_STATE_WAITING:
              // do nothing until state changed
              break;
            case SDCARD_STATE_TIMEOUT:

                USART1_Write("File Operation TIMEOUT!\r\n",sizeof("File Operation TIMEOUT!\r\n"));
                SdcardData->state = SDCARD_STATE_WAITING;
              break;

            default:
            {
            }
            break;
        }
}

void SDCARD_WriteorRead(SDCARD_DATA* SdcardData,bool ifwrite){
  SdcardData->WorR = ifwrite;
}

void SDCARD_FileName(SDCARD_DATA* SdcardData,char* fileName){
    SdcardData->fileOpen = fileName;

}

void SDCARD_StateSwitch(SDCARD_DATA* SdcardData,SDCARD_STATES state){
    SdcardData->state = state;
}

void SDCARD_FillinBuffer(SDCARD_DATA* SdcardData,uint8_t* buf,size_t nbytes){
    SdcardData->buf= buf;
    SdcardData->nbytes = nbytes;
    SdcardData->bufFilled = true;
}

void SDCARD_SetDevice(SDCARD_DATA* SdcardData,char* devName){
  SdcardData->devName = devName;
}

void SDCARD_SetMount(SDCARD_DATA* SdcardData,char* mntName){
  SdcardData->mntName = mntName;
}
