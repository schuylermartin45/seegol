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

/** Macros     **/

// Video text memory boundaries
#define TEXT_WIDTH     80
#define TEXT_HEIGHT    25
#define TEXT_MEM_SIZE  (TEXT_WIDTH * TEXT_HEIGHT * 2) // 2 bytes per char
#define TEXT_MEM_BEGIN 0xB8000
#define TEXT_MEM_END   (TEXT_MEM_BEGIN + TEXT_MEM_END)

// various text color modes
#define KIO_WHITE_ON_BLACK  0x07
#define KIO_DEFAULT_COLOR   KIO_WHITE_ON_BLACK

/** Globals    **/

/** Structures **/

/** Functions  **/

/*
** Kernel print: prints chars to the screen
**
** @param str String to print
*/
void k_print(const char* str);

/*
** Kernel printf: k_print with some functionality of the STDIO printf
**
** @param str String to print
** @param TODO
*/
void k_printf(const char* str);

#endif
