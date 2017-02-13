/*
** File:    seesh.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: SeeSH shell program for SeeGOL
*/
#ifndef _SEESH_H_
#define _SEESH_H_

/** Headers    **/
#include "../kern/gcc16.h"
#include "../kern/types.h"
#include "../kern/debug.h"
#include "../kern/kio.h"
// definitions for all programs
#include "program.h"
// user program headers
#include "hellow.h"
#include "hsc_tp.h"

/** Macros     **/
// shell macros
#define SHELL_PROMPT    "seesh> "   // SeeGOL + shell = seesh
#define SHELL_BUFF_SIZE 80
#define SHELL_ARG_SIZE  32
#define MSG_SHELL_START "SeeSH (Shoyler's Extremely Experimental SHell)\n"

// shell failure exit code
#define SHELL_FAIL_EXIT 42

/** Globals    **/

/** Structures **/

/** Functions  **/

/*
** Main execution point of the shell
*/
uint8_t seesh_main(void);

#endif
