/*
** File:    hsc_tp.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Graphical test program that displays the HSC logo and a test
**              pattern
*/

#ifndef _HSC_TP_H_
#define _HSC_TP_H_

/** Headers    **/
#include "../kern/gcc16.h"
#include "../kern/types.h"
#include "../kern/kio.h"
#include "../gl/gl_lib.h"
#include "../gl/img_fids.h"
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
void hsc_tp_init(Program* prog);

/*
** Main method for HSC Test Pattern program
*/
uint8_t hsc_tp_main(uint8_t argc, char* argv[]);

#endif
