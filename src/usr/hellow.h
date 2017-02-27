/*
** File:    hellow.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Hello World demo program
*/
#ifndef _HELLOW_H_
#define _HELLOW_H_

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
void hellow_init(Program* prog);

/*
** Main method for hello world program
*/
uint8_t hellow_main(uint8_t argc, char* argv[]);

#endif
