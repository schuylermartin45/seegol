/*
** File:    slideshow.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Slideshow program that displays images to the screen
*/

#ifndef _SLIDESHOW_H_
#define _SLIDESHOW_H_

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
void slideshow_init(Program* prog);

/*
** Main method for the program
*/
uint8_t slideshow_main(uint8_t argc, char* argv[]);

#endif
