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

static bool SDCARD_Read_SDCard(
    const DRV_HANDLE handle,
    uint8_t* const pBuffer,
    const uint16_t requestedBytes,
    uint16_t* const pNumBytesRead
)
{
    uint32_t nBytesRead = 0;
    bool isSuccess = true;

    /* Read audio text data from SD card*/
    nBytesRead = SYS_FS_FileRead( handle, (void*)pBuffer, requestedBytes );

    if (-1 == (int32_t)nBytesRead)
    {
        if ( false == SYS_FS_FileEOF(handle))
        {
            /* We have encountered a file read error */
            isSuccess = false;
        }
    }

    *pNumBytesRead = (uint16_t)nBytesRead;

    return isSuccess;
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
            {
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
            }
            break;

            case SDCARD_STATE_WRITE_FILE:
            {
              SdcardData.fileHandle = SYS_FS_FileOpen(SdcardData.fileOpen, SYS_FS_FILE_OPEN_APPEND);
              if(SdcardData.fileHandle == SYS_FS_HANDLE_INVALID)
              {
                  /* Could not open the file. Error out*/
                  SdcardData.state = SDCARD_STATE_ERROR;
              }
              else
              {
                bytes_written = SYS_FS_FileWrite(SdcardData.fileHandle, (const void *)SdcardData.buf, SdcardData.nbytes);
                if (bytes_written != -1){
                  SdcardData.state = SDCARD_STATE_WAITING;
                }else{
                  SdcardData.state = SDCARD_STATE_ERROR;
                }
              SYS_FS_FileClose(handle);
              }

            }
            break;

            case SDCARD_STATE_WAITING:
            {
              // do nothing until state changed
            }
            break;

            case SDCARD_STATE_READ_FILE:
            {
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




                // LAST END POINT TO BE EDITED
            case SDCARD_STATE_CARD_READ:
            {
                uint16_t nBytesRead = 0;
                uint16_t nBytesParsed = 0;
                uint16_t nRequestedBytes = 0;

                if (SdcardData.currentFilePosition < SdcardData.fileSize)
                {
                    /* Read audio text data from SD card*/

                    nRequestedBytes =
                       sizeof(SdcardData.textParser.buffer) - SdcardData.textParser.nElements;

                    if (true == SDCARD_Read_SDCard(
                            SdcardData.fileHandle,
                            &SdcardData.textParser.buffer[SdcardData.textParser.nElements],
                            nRequestedBytes,
                            &nBytesRead )
                        )
                    {
                        SdcardData.textParser.nElements += nBytesRead;

                        /* Progress the file position */
                        SdcardData.currentFilePosition += nBytesRead;

                        /* Parse the read audio text data */
                        SdcardData.codec.bufferSize +=
                            SDCARD_Parse_Audio_Text_Data(
                                SdcardData.textParser.buffer,
                                (int16_t* const)(&((uint8_t*)SdcardData.codec.txbufferObject)[SdcardData.codec.bufferSize]),
                                SdcardData.textParser.nElements,
                                &nBytesParsed
                            );

                        /* Copy the left over (not parsed) data to the start of the textParser input buffer */
                        memcpy(
                            SdcardData.textParser.buffer,
                            &SdcardData.textParser.buffer[nBytesParsed],
                            (SdcardData.textParser.nElements - nBytesParsed)
                        );

                        /* Set the index into the textDataBufferSize to reflect the copied left over data */
                        SdcardData.textParser.nElements -= nBytesParsed;

                        if (nBytesRead != nRequestedBytes)
                        {
                            /* We have reached End of File. Close it and submit the buffer for playing */
                            SYS_FS_FileClose(SdcardData.fileHandle);
                            SdcardData.state = SDCARD_STATE_CODEC_ADD_BUFFER;
                        }
                    }
                    else
                    {
                        /* We have a file read error. Close the file */
                        SYS_FS_FileClose(SdcardData.fileHandle);
                        SdcardData.state = SDCARD_STATE_ERROR;
                    }
                }
                else
                {
                    /* The file is read. Close it and submit the buffer for playing */
                    SYS_FS_FileClose(SdcardData.fileHandle);
                    SdcardData.state = SDCARD_STATE_CODEC_ADD_BUFFER;
                }

            }

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
