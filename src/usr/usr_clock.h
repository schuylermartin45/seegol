/*
** File:    usr_clock.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: User program that displays RTC time
*/

#ifndef _USR_CLOCK_H_
#define _USR_CLOCK_H_

/** Headers    **/
#include "../kern/gcc16.h"
#include "../kern/types.h"
#include "program.h"

/** Macros     **/

/** Globals    **/

/** Structures **/

/** Functions  **/

/*
** Initializes program structure
**
** @param prog Program pointer to set
*/
void usr_clock_init(Program* prog);

/*
** Main method for the program
*/
uint8_t usr_clock_main(uint8_t argc, char* argv[]);

#endif
