/*
** File:    types.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Header file that contains type macro definitions
*/
#ifndef _TYPES_H_
#define _TYPES_H_

/** Headers    **/
#include "gcc16.h"

// Numeric types
typedef char            int8_t;
typedef unsigned char   uint8_t;
typedef uint8_t         byte_t;
typedef short           int16_t;
typedef unsigned short  uint16_t;

// bit masks that for loading the lower/upper byte of a short (16 bits)
#define LOAD_LO_BYTE_MASK 0xFF
#define LOAD_HI_BYTE_MASK 0xFF00

// Boolean types
#ifndef false
    #define false 0
#endif
#ifndef true
    #define true 1
#endif
// null
#ifndef NULL
    #define NULL 0
#endif

// String macros to allow concatenation of other string macros
#define STR_HELP(x) #x
#define STR(x) STR_HELP(x)


#endif
