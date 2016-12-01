/*
** File:    asm_lib.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: x86 Assembly support library
*/

/** Headers    **/
#include "gcc16.h"
#include "asm_lib.h"

/*
** Puts a char on the screen
**
** @param chr Char to print
*/
void __put_chr(const char chr)
{
    __asm__ __volatile__ ("int $0x10" : : "a"(0x0E00 | chr), "b"(7));
}
