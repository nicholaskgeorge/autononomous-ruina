/*******************************************************************************
  USART0 SPI PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_usart0_spi.c

  Summary:
    USART0 SPI PLIB Implementation File

  Description:
    None

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
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
#include "plib_usart0_spi.h"
#include "interrupts.h"

// *****************************************************************************
// *****************************************************************************
// Section: USART0 SPI Implementation
// *****************************************************************************
// *****************************************************************************
static USART_SPI_OBJECT usart0SPIObj;


void USART0_SPI_Initialize( void )
{
    /* Configure USART0 mode to SPI Master (0x0E) */
    USART0_REGS->US_MR = US_MR_SPI_USART_MODE(US_MR_SPI_USART_MODE_SPI_MASTER_Val);

    /* Reset SPI RX, SPI TX and SPI status */
    USART0_REGS->US_CR = (US_CR_SPI_RSTRX_Msk | US_CR_SPI_RSTTX_Msk | US_CR_SPI_RSTSTA_Msk);

    /* Configure clock source, clock phase, clock polarity and CKO = 1 */
    USART0_REGS->US_MR |= (US_MR_USART_USCLKS_MCK | US_MR_SPI_CHRL(US_MR_SPI_CHRL_8_BIT_Val) | US_MR_SPI_CPHA(0x0) | US_MR_SPI_CPOL(0x0) | US_MR_SPI_CLKO(1));

    /* Enable TX and RX */
    USART0_REGS->US_CR = (US_CR_SPI_RXEN_Msk | US_CR_SPI_TXEN_Msk);

    /* Configure USART0 Baud Rate */
    USART0_REGS->US_BRGR = US_BRGR_CD(150);

    /* Initialize instance object */
    usart0SPIObj.callback = NULL;
    usart0SPIObj.context = (uintptr_t)0;
    usart0SPIObj.transferIsBusy = false;
}

bool USART0_SPI_TransferSetup( USART_SPI_TRANSFER_SETUP * setup, uint32_t spiSourceClock )
{
    uint32_t clockDivider = 0;

    if ((setup == NULL) || (setup->clockFrequency == 0))
    {
        return false;
    }

    if(spiSourceClock == 0)
    {
        // Fetch Master Clock Frequency directly
        spiSourceClock = 150000000UL;
    }

    clockDivider = spiSourceClock/setup->clockFrequency;

    if(clockDivider < 6)
    {
        clockDivider = 6;
    }
    else if(clockDivider > 65535)
    {
        clockDivider = 65535;
    }

    USART0_REGS->US_MR = ((USART0_REGS->US_MR & ~US_MR_SPI_CPOL_Msk) | (uint32_t)setup->clockPolarity);
    USART0_REGS->US_MR = ((USART0_REGS->US_MR & ~US_MR_SPI_CPHA_Msk) | (uint32_t)setup->clockPhase);
    USART0_REGS->US_MR = ((USART0_REGS->US_MR & ~US_MR_SPI_CHRL_Msk) | (uint32_t)setup->dataBits);

    USART0_REGS->US_BRGR = ((USART0_REGS->US_BRGR & ~US_BRGR_CD_Msk) | US_BRGR_CD(clockDivider));

    return true;
}

bool USART0_SPI_WriteRead( void* pTransmitData, size_t txSize, void* pReceiveData, size_t rxSize )
{
    bool isRequestAccepted = false;
    uint32_t dummyData;

    /* Verify the request */
    if((((txSize > 0) && (pTransmitData != NULL)) || ((rxSize > 0) && (pReceiveData != NULL))) && (usart0SPIObj.transferIsBusy == false))
    {
        isRequestAccepted = true;

        usart0SPIObj.transferIsBusy = true;
        usart0SPIObj.txBuffer = pTransmitData;
        usart0SPIObj.rxBuffer = pReceiveData;
        usart0SPIObj.rxCount = 0;
        usart0SPIObj.txCount = 0;
        usart0SPIObj.dummySize = 0;

        if (pTransmitData != NULL)
        {
            usart0SPIObj.txSize = txSize;
        }
        else
        {
            usart0SPIObj.txSize = 0;
        }

        if (pReceiveData != NULL)
        {
            usart0SPIObj.rxSize = rxSize;
        }
        else
        {
            usart0SPIObj.rxSize = 0;
        }

        /* Reset over-run error if any */
        USART0_REGS->US_CR = US_CR_SPI_RSTSTA_Msk;

        /* Flush out any unread data in SPI read buffer */
        if (USART0_REGS->US_CSR & US_CSR_SPI_RXRDY_Msk)
        {
            dummyData = USART0_REGS->US_RHR;
            (void)dummyData;
        }

        if (usart0SPIObj.rxSize > usart0SPIObj.txSize)
        {
            usart0SPIObj.dummySize = usart0SPIObj.rxSize - usart0SPIObj.txSize;
        }

        /* Start the first write here itself, rest will happen in ISR context */
        if ((USART0_REGS->US_MR & US_MR_SPI_CHRL_Msk) == US_MR_SPI_CHRL_8_BIT)
        {

            if (usart0SPIObj.txCount < usart0SPIObj.txSize)
            {
                USART0_REGS->US_THR = *((uint8_t*)usart0SPIObj.txBuffer);
                usart0SPIObj.txCount++;
            }
            else if (usart0SPIObj.dummySize > 0)
            {
                USART0_REGS->US_THR = (uint8_t)(0xff);
                usart0SPIObj.dummySize--;
            }
        }

        if (rxSize > 0)
        {
            /* Enable receive interrupt to complete the transfer in ISR context */
            USART0_REGS->US_IER = US_IER_SPI_RXRDY_Msk;
        }
        else
        {
            /* Enable transmit interrupt to complete the transfer in ISR context */
            USART0_REGS->US_IER = US_IER_SPI_TXRDY_Msk;
        }
    }

    return isRequestAccepted;
}

bool USART0_SPI_Write( void* pTransmitData, size_t txSize )
{
    return(USART0_SPI_WriteRead(pTransmitData, txSize, NULL, 0));
}

bool USART0_SPI_Read( void* pReceiveData, size_t rxSize )
{
    return(USART0_SPI_WriteRead(NULL, 0, pReceiveData, rxSize));
}

bool USART0_SPI_IsBusy( void )
{
    return ((usart0SPIObj.transferIsBusy) || ((USART0_REGS->US_CSR & US_CSR_SPI_TXEMPTY_Msk) == 0));
}

void USART0_SPI_CallbackRegister( USART_SPI_CALLBACK callback, uintptr_t context )
{
    usart0SPIObj.callback = callback;
    usart0SPIObj.context = context;
}

void USART0_InterruptHandler( void )
{
    uint32_t receivedData;


    if (USART0_REGS->US_CSR & US_CSR_SPI_RXRDY_Msk)
    {
        receivedData = USART0_REGS->US_RHR;

        if (usart0SPIObj.rxCount < usart0SPIObj.rxSize)
        {
            ((uint8_t*)usart0SPIObj.rxBuffer)[usart0SPIObj.rxCount++] = receivedData;
        }
    }

    if(USART0_REGS->US_CSR & US_CSR_SPI_TXRDY_Msk)
    {
        /* Disable the TXRDY interrupt. This will be enabled back if one or more
         * bytes are pending transmission */

        USART0_REGS->US_IDR = US_IDR_SPI_TXRDY_Msk;

        if (usart0SPIObj.txCount < usart0SPIObj.txSize)
        {
            USART0_REGS->US_THR = ((uint8_t*)usart0SPIObj.txBuffer)[usart0SPIObj.txCount++];
        }
        else if (usart0SPIObj.dummySize > 0)
        {
            USART0_REGS->US_THR = (uint8_t)(0xff);
            usart0SPIObj.dummySize--;
        }

        if ((usart0SPIObj.txCount == usart0SPIObj.txSize) && (usart0SPIObj.dummySize == 0))
        {
            if (USART0_REGS->US_CSR & US_CSR_SPI_TXEMPTY_Msk)
            {
                /* Disable all interrupt sources - RXRDY, TXRDY and TXEMPTY */
                USART0_REGS->US_IDR = (US_IDR_SPI_RXRDY_Msk | US_IDR_SPI_TXRDY_Msk | US_IDR_SPI_TXEMPTY_Msk);

                usart0SPIObj.transferIsBusy = false;


                /* All characters are transmitted out and TX shift register is empty */
                if(usart0SPIObj.callback != NULL)
                {
                    usart0SPIObj.callback(usart0SPIObj.context);
                }
            }
            else
            {
                /* Enable TXEMPTY interrupt */
                USART0_REGS->US_IER = US_IER_SPI_TXEMPTY_Msk;
            }
        }
        else if (usart0SPIObj.rxCount == usart0SPIObj.rxSize)
        {
            /* Enable TXRDY interrupt as all the requested bytes are received
             * and can now make use of the internal transmit shift register.
             */
            USART0_REGS->US_IDR = US_IDR_SPI_RXRDY_Msk;
            USART0_REGS->US_IER = US_IER_SPI_TXRDY_Msk;
        }
    }
}
