/*
** File:    trench_run.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: These plans are NOT in the main computer
*/
#ifndef _TRENCH_RUN_H_
#define _TRENCH_RUN_H_

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
void trench_run_init(Program* prog);

/*
** Main method for the program
*/
uint8_t trench_run_main(uint8_t argc, char* argv[]);

#endif
