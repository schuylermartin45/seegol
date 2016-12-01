/*
** File:    kio.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Kernel I/O for basic debugging purposes
*/
#ifndef _KIO_H_
#define _KIO_H_

/** Headers    **/
#include "gcc16.h"
#include "asm_lib.h"

/** Macros     **/

/** Globals    **/

/** Structures **/

/** Functions  **/

/*
** Kernel print: prints chars to the screen
**
** @param str String to print
*/
void k_print(const char *str);

/*
** Kernel printf: k_print with some functionality of the STDIO printf
**
** @param str String to print
** @param TODO
*/
void k_printf(const char *str);

#endif
