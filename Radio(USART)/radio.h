/*
 * File:   radio.h
 * Anemometer library
 * Author: Nicholas George
 *
 * Created on August 26, 2020, 3:38 PM
 */

#ifndef RADIO_H
#define	RADIO_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <stdio.h>
#include <string.h>

int Radio_Encode(uint8_t* data, uint8_t* buffer, int size);
int Radio_Decode(uint8_t* data, uint8_t* buffer);

#ifdef	__cplusplus
}
#endif

#endif	/* RADIO_H */