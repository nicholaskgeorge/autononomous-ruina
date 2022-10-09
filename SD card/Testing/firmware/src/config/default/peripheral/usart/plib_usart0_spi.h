/*******************************************************************************
  USART0 SPI PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_usart0_spi.h

  Summary:
    USART0 SPI PLIB Header File

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

#ifndef PLIB_USART0_SPI_H
#define PLIB_USART0_SPI_H

#include "plib_usart_spi_common.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Interface
// *****************************************************************************
// *****************************************************************************

/****************************** USART0 SPI API *********************************/

void USART0_SPI_Initialize( void );

bool USART0_SPI_TransferSetup( USART_SPI_TRANSFER_SETUP * setup, uint32_t spiSourceClock );

bool USART0_SPI_WriteRead( void* pTransmitData, size_t txSize, void* pReceiveData, size_t rxSize );

bool USART0_SPI_Write( void* pTransmitData, size_t txSize );

bool USART0_SPI_Read( void* pReceiveData, size_t rxSize );

bool USART0_SPI_IsBusy( void );

void USART0_SPI_CallbackRegister( USART_SPI_CALLBACK callback, uintptr_t context );

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

// DOM-IGNORE-END
#endif // PLIB_USART0_SPI_H
