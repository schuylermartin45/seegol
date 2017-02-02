/*
** File:    asm_lib.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: x86 Assembly support library
*/
#ifndef _ASM_LIB_H_
#define _ASM_LIB_H_

/** Headers    **/
#include "gcc16.h"
#include "types.h"

/** Macros     **/

/** Globals    **/

/** Structures **/

/** Functions  **/

/*
** Port I/O: Input Byte
**
** @param port Comm port
*/
uint8_t __inb(uint16_t port);

/*
** Port I/O: Output Byte
**
** @param port Comm port
** @param val Value to emit
*/
void __outb(uint8_t port, uint8_t val);

#endif
