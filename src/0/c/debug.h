/*
** File:    debug.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Header file that defines basic debug macros
*/

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

/********** Macros     **********/
#define DEBUG_MODE

#ifdef DEBUG_MODE
    #ifndef DEBUG_OUT
        #define DEBUG_OUT(x) printf("%s\n", x)
    #endif

    #ifndef DEBUG_CHR
        #define DEBUG_CHR(x) printf("DEBUG: %s = %c\n", #x, x)
    #endif

    #ifndef DEBUG_STR
        #define DEBUG_STR(x) printf("DEBUG: %s = %s\n", #x, x)
    #endif

    #ifndef DEBUG_INT
        #define DEBUG_INT(x) printf("DEBUG: %s = %d\n", #x, x)
    #endif

    #ifndef DEBUG_HEX
        #define DEBUG_HEX(x) printf("DEBUG: %s = 0x%x\n", #x, x)
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

/********** Globals    **********/

/********** Structures **********/

/********** Functions  **********/

#endif
