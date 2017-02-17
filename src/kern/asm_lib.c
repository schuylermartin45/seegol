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
** Port I/O: Input Byte
**
** @param port Comm port
*/
uint8_t _inb(uint16_t port)
{
    uint8_t val;
    __asm__ __volatile__("inb %1, %0"
        : "=a"(val)
        : "Nd"(port)
    );
    return val;
}

/*
** Port I/O: Output Byte
**
** @param port Comm port
** @param val Value to emit
*/
void _outb(uint16_t port, uint8_t val)
{
    __asm__ __volatile__( "outb %0, %1" : : "a"(val), "Nd"(port));
}
