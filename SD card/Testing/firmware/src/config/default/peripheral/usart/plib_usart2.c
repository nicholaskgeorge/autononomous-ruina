/*******************************************************************************
  USART2 PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_usart2.c

  Summary:
    USART2 PLIB Implementation File

  Description:
    None

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#include "device.h"
#include "plib_usart2.h"
#include "interrupts.h"

// *****************************************************************************
// *****************************************************************************
// Section: USART2 Implementation
// *****************************************************************************
// *****************************************************************************

static void USART2_ErrorClear( void )
{
    uint8_t dummyData = 0u;

   if (USART2_REGS->US_CSR & (US_CSR_USART_OVRE_Msk | US_CSR_USART_PARE_Msk | US_CSR_USART_FRAME_Msk))
   {
        /* Clear the error flags */
        USART2_REGS->US_CR = US_CR_USART_RSTSTA_Msk;

        /* Flush existing error bytes from the RX FIFO */
        while (USART2_REGS->US_CSR & US_CSR_USART_RXRDY_Msk)
        {
            dummyData = (USART2_REGS->US_RHR & US_RHR_RXCHR_Msk);
        }
   }

    /* Ignore the warning */
    (void)dummyData;
}


USART_OBJECT usart2Obj;

static void USART2_ISR_RX_Handler( void )
{
    uint16_t rxData = 0;

    if(usart2Obj.rxBusyStatus == true)
    {
        while((USART2_REGS->US_CSR & US_CSR_USART_RXRDY_Msk) && (usart2Obj.rxSize > usart2Obj.rxProcessedSize))
        {
            rxData = USART2_REGS->US_RHR & US_RHR_RXCHR_Msk;
            if (USART2_REGS->US_MR & US_MR_USART_MODE9_Msk)
            {
                ((uint16_t*)usart2Obj.rxBuffer)[usart2Obj.rxProcessedSize++] = (uint16_t)rxData;
            }
            else
            {
                usart2Obj.rxBuffer[usart2Obj.rxProcessedSize++] = (uint8_t)rxData;
            }
        }

        /* Check if the buffer is done */
        if(usart2Obj.rxProcessedSize >= usart2Obj.rxSize)
        {
            usart2Obj.rxBusyStatus = false;

            /* Disable Read, Overrun, Parity and Framing error interrupts */
            USART2_REGS->US_IDR = (US_IDR_USART_RXRDY_Msk | US_IDR_USART_FRAME_Msk | US_IDR_USART_PARE_Msk | US_IDR_USART_OVRE_Msk);

            if(usart2Obj.rxCallback != NULL)
            {
                usart2Obj.rxCallback(usart2Obj.rxContext);
            }
        }
    }
    else
    {
        /* Nothing to process */
        ;
    }
}

static void USART2_ISR_TX_Handler( void )
{
    if(usart2Obj.txBusyStatus == true)
    {
        while((USART2_REGS->US_CSR & US_CSR_USART_TXRDY_Msk) && (usart2Obj.txSize > usart2Obj.txProcessedSize))
        {
            if (USART2_REGS->US_MR & US_MR_USART_MODE9_Msk)
            {
                USART2_REGS->US_THR = ((uint16_t*)usart2Obj.txBuffer)[usart2Obj.txProcessedSize++] & US_THR_TXCHR_Msk;
            }
            else
            {
                USART2_REGS->US_THR = usart2Obj.txBuffer[usart2Obj.txProcessedSize++] & US_THR_TXCHR_Msk;
            }
        }

        /* Check if the buffer is done */
        if(usart2Obj.txProcessedSize >= usart2Obj.txSize)
        {
            usart2Obj.txBusyStatus = false;

            USART2_REGS->US_IDR = US_IDR_USART_TXRDY_Msk;

            if(usart2Obj.txCallback != NULL)
            {
                usart2Obj.txCallback(usart2Obj.txContext);
            }
        }
    }
    else
    {
        /* Nothing to process */
        ;
    }
}

void USART2_InterruptHandler( void )
{
    /* Error status */
    uint32_t errorStatus = (USART2_REGS->US_CSR & (US_CSR_USART_OVRE_Msk | US_CSR_USART_FRAME_Msk | US_CSR_USART_PARE_Msk));

    if(errorStatus != 0)
    {
        /* Save the error to be reported later */
        usart2Obj.errorStatus = (USART_ERROR)errorStatus;

        /* Clear error flags and flush the error data */
        USART2_ErrorClear();

        /* Disable Read, Overrun, Parity and Framing error interrupts */
        USART2_REGS->US_IDR = (US_IDR_USART_RXRDY_Msk | US_IDR_USART_FRAME_Msk | US_IDR_USART_PARE_Msk | US_IDR_USART_OVRE_Msk);

        usart2Obj.rxBusyStatus = false;

        /* USART errors are normally associated with the receiver, hence calling
         * receiver callback */
        if( usart2Obj.rxCallback != NULL )
        {
            usart2Obj.rxCallback(usart2Obj.rxContext);
        }
    }

    /* Receiver status */
    if (USART2_REGS->US_CSR & US_CSR_USART_RXRDY_Msk)
    {
        USART2_ISR_RX_Handler();
    }

    /* Transmitter status */
    if ( (USART2_REGS->US_CSR & US_CSR_USART_TXRDY_Msk) && (USART2_REGS->US_IMR & US_IMR_USART_TXRDY_Msk) )
    {
        USART2_ISR_TX_Handler();
    }
}

void USART2_Initialize( void )
{
    /* Reset USART2 */
    USART2_REGS->US_CR = (US_CR_USART_RSTRX_Msk | US_CR_USART_RSTTX_Msk | US_CR_USART_RSTSTA_Msk);

    /* Enable USART2 */
    USART2_REGS->US_CR = (US_CR_USART_TXEN_Msk | US_CR_USART_RXEN_Msk);

    /* Configure USART2 mode */
    USART2_REGS->US_MR = (US_MR_USART_USCLKS_MCK | US_MR_USART_CHRL_8_BIT | US_MR_USART_PAR_NO | US_MR_USART_NBSTOP_1_BIT | (0 << US_MR_USART_OVER_Pos));

    /* Configure USART2 Baud Rate */
    USART2_REGS->US_BRGR = US_BRGR_CD(81);

    /* Initialize instance object */
    usart2Obj.rxBuffer = NULL;
    usart2Obj.rxSize = 0;
    usart2Obj.rxProcessedSize = 0;
    usart2Obj.rxBusyStatus = false;
    usart2Obj.rxCallback = NULL;
    usart2Obj.txBuffer = NULL;
    usart2Obj.txSize = 0;
    usart2Obj.txProcessedSize = 0;
    usart2Obj.txBusyStatus = false;
    usart2Obj.txCallback = NULL;
    usart2Obj.errorStatus = USART_ERROR_NONE;
}

USART_ERROR USART2_ErrorGet( void )
{
    USART_ERROR errors = usart2Obj.errorStatus;

    usart2Obj.errorStatus = USART_ERROR_NONE;

    /* All errors are cleared, but send the previous error state */
    return errors;
}

bool USART2_SerialSetup( USART_SERIAL_SETUP *setup, uint32_t srcClkFreq )
{
    uint32_t baud = setup->baudRate;
    uint32_t brgVal = 0;
    uint32_t overSampVal = 0;
    uint32_t usartMode;
    bool status = false;

    if((usart2Obj.rxBusyStatus == true) || (usart2Obj.txBusyStatus == true))
    {
        /* Transaction is in progress, so return without updating settings */
        return false;
    }

    if (setup != NULL)
    {
        baud = setup->baudRate;
        if(srcClkFreq == 0)
        {
            srcClkFreq = USART2_FrequencyGet();
        }

        /* Calculate BRG value */
        if (srcClkFreq >= (16 * baud))
        {
            brgVal = (srcClkFreq / (16 * baud));
        }
        else if (srcClkFreq >= (8 * baud))
        {
            brgVal = (srcClkFreq / (8 * baud));
            overSampVal = US_MR_USART_OVER(1);
        }
        else
        {
            return false;
        }

        if (brgVal > 65535)
        {
            /* The requested baud is so low that the ratio of srcClkFreq to baud exceeds the 16-bit register value of CD register */
            return false;
        }

        /* Configure USART2 mode */
        usartMode = USART2_REGS->US_MR;
        usartMode &= ~(US_MR_USART_CHRL_Msk | US_MR_USART_MODE9_Msk | US_MR_USART_PAR_Msk | US_MR_USART_NBSTOP_Msk | US_MR_USART_OVER_Msk);
        USART2_REGS->US_MR = usartMode | ((uint32_t)setup->dataWidth | (uint32_t)setup->parity | (uint32_t)setup->stopBits | (uint32_t)overSampVal);

        /* Configure USART2 Baud Rate */
        USART2_REGS->US_BRGR = US_BRGR_CD(brgVal);
        status = true;
    }

    return status;
}

bool USART2_Read( void *buffer, const size_t size )
{
    bool status = false;
    uint8_t* pBuffer = (uint8_t *)buffer;

    if(pBuffer != NULL)
    {
        /* Check if receive request is in progress */
        if(usart2Obj.rxBusyStatus == false)
        {
            /* Clear errors that may have got generated when there was no active read request pending */
            USART2_ErrorClear();

            /* Clear the errors related to pervious read requests */
            usart2Obj.errorStatus = USART_ERROR_NONE;

            usart2Obj.rxBuffer = pBuffer;
            usart2Obj.rxSize = size;
            usart2Obj.rxProcessedSize = 0;
            usart2Obj.rxBusyStatus = true;

            status = true;

            /* Enable Read, Overrun, Parity and Framing error interrupts */
            USART2_REGS->US_IER = (US_IER_USART_RXRDY_Msk | US_IER_USART_FRAME_Msk | US_IER_USART_PARE_Msk | US_IER_USART_OVRE_Msk);
        }
    }

    return status;
}

bool USART2_Write( void *buffer, const size_t size )
{
    bool status = false;
    uint8_t* pBuffer = (uint8_t *)buffer;

    if(NULL != pBuffer)
    {
        /* Check if transmit request is in progress */
        if(usart2Obj.txBusyStatus == false)
        {
            usart2Obj.txBuffer = pBuffer;
            usart2Obj.txSize = size;
            usart2Obj.txProcessedSize = 0;
            usart2Obj.txBusyStatus = true;
            status = true;

            /* Initiate the transfer by writing as many bytes as possible */
            while ((USART2_REGS->US_CSR & US_CSR_USART_TXRDY_Msk) && (usart2Obj.txProcessedSize < usart2Obj.txSize))
            {
                if (USART2_REGS->US_MR & US_MR_USART_MODE9_Msk)
                {
                    USART2_REGS->US_THR = ((uint16_t*)pBuffer)[usart2Obj.txProcessedSize++] & US_THR_TXCHR_Msk;
                }
                else
                {
                    USART2_REGS->US_THR = pBuffer[usart2Obj.txProcessedSize++] & US_THR_TXCHR_Msk;
                }
            }

            USART2_REGS->US_IER = US_IER_USART_TXRDY_Msk;

        }
    }

    return status;
}


void USART2_WriteCallbackRegister( USART_CALLBACK callback, uintptr_t context )
{
    usart2Obj.txCallback = callback;

    usart2Obj.txContext = context;
}

void USART2_ReadCallbackRegister( USART_CALLBACK callback, uintptr_t context )
{
    usart2Obj.rxCallback = callback;

    usart2Obj.rxContext = context;
}

bool USART2_WriteIsBusy( void )
{
    return usart2Obj.txBusyStatus;
}

bool USART2_ReadIsBusy( void )
{
    return usart2Obj.rxBusyStatus;
}

bool USART2_ReadAbort(void)
{
    if (usart2Obj.rxBusyStatus == true)
    {
        /* Disable Read, Overrun, Parity and Framing error interrupts */
        USART2_REGS->US_IDR = (US_IDR_USART_RXRDY_Msk | US_IDR_USART_FRAME_Msk | US_IDR_USART_PARE_Msk | US_IDR_USART_OVRE_Msk);

        usart2Obj.rxBusyStatus = false;

        /* If required application should read the num bytes processed prior to calling the read abort API */
        usart2Obj.rxSize = usart2Obj.rxProcessedSize = 0;
    }

    return true;
}

size_t USART2_WriteCountGet( void )
{
    return usart2Obj.txProcessedSize;
}

size_t USART2_ReadCountGet( void )
{
    return usart2Obj.rxProcessedSize;
}

