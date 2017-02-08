/*
** File:    program.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Defines a user-level program
*/
#ifndef _PROGRAM_H_
#define _PROGRAM_H_

/** Headers    **/
#include "../kern/gcc16.h"
#include "../kern/types.h"

/** Macros     **/
// program error codes
#define ERR_PROG_NOT_FOUND  1
#define ERR_PROG_GEN_FAIL   2
#define ERR_PROG_USAGE      3
// and when you get it right
#define EXIT_SUCCESS        0

/** Globals    **/

/** Structures **/
typedef struct Program
{
    // info on the program
    char* name;
    char* desc;
    // main method of the program
    uint16_t (*main)(uint16_t argc, char* argv);
} Program;

#endif
