/*
** File:    debug.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Header file that contains debug macros
*/
#ifndef _DEBUG_H_
#define _DEBUG_H_

/** Headers    **/
#include "gcc16.h"
#include "kio.h"

#define DEBUG_MODE

#ifdef DEBUG_MODE
    #ifndef DEBUG_CHR
        #define DEBUG_CHR(x) kio_printf("DEBUG: %s = %c\n", #x, &x)
    #endif

    #ifndef DEBUG_STR
        #define DEBUG_STR(x) kio_printf("DEBUG: %s = %s\n", #x, x)
    #endif
    
    #ifndef DEBUG_BIN
        #define DEBUG_BIN(x) kio_printf("DEBUG: %s = %b\n", #x, &x)
    #endif

    #ifndef DEBUG_INT
        #define DEBUG_INT(x) kio_printf("DEBUG: %s = %d\n", #x, &x)
    #endif

    #ifndef DEBUG_HEX
        #define DEBUG_HEX(x) kio_printf("DEBUG: %s = %x\n", #x, &x)
    #endif
// replace with nothing if debug mode is off
#else
    #ifndef DEBUG_CHR
        #define DEBUG_CHR(x)
    #endif

    #ifndef DEBUG_STR
        #define DEBUG_STR(x)
    #endif

    #ifndef DEBUG_BIN
        #define DEBUG_BIN(x)
    #endif

    #ifndef DEBUG_INT
        #define DEBUG_INT(x)
    #endif

    #ifndef DEBUG_HEX
        #define DEBUG_HEX(x)
    #endif
#endif

#endif
