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
    #ifndef DEBUG_OUT
        #define DEBUG_OUT(x) k_printf("%s\n", x)
    #endif

    #ifndef DEBUG_CHR
        #define DEBUG_CHR(x) k_printf("DEBUG: %s = %c\n", #x, x)
    #endif

    #ifndef DEBUG_STR
        #define DEBUG_STR(x) k_printf("DEBUG: %s = %s\n", #x, x)
    #endif

    #ifndef DEBUG_INT
        #define DEBUG_INT(x) k_printf("DEBUG: %s = %d\n", #x, x)
    #endif

    #ifndef DEBUG_HEX
        #define DEBUG_HEX(x) k_printf("DEBUG: %s = 0x%x\n", #x, x)
    #endif
// replace with nothing if debug mode is off
#else
    #ifndef DEBUG_OUT
        #define DEBUG_OUT(x)
    #endif

    #ifndef DEBUG_CHR
        #define DEBUG_CHR(x)
    #endif

    #ifndef DEBUG_STR
        #define DEBUG_STR(x)
    #endif

    #ifndef DEBUG_INT
        #define DEBUG_INT(x)
    #endif

    #ifndef DEBUG_HEX
        #define DEBUG_HEX(x)
    #endif
#endif


#endif
