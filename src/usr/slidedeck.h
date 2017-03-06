/*
** File:    slidedeck.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Slidedeck program that displays panes, like a poor man's power
**              point; a presentation
*/

#ifndef _SLIDEDECK_H_
#define _SLIDEDECK_H_

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
void slidedeck_init(Program* prog);

/*
** Main method for HSC Test Pattern program
*/
uint8_t slidedeck_main(uint8_t argc, char* argv[]);

#endif
