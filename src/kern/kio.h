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
#include "types.h"

/** Macros     **/

// Video text memory boundaries
#define TEXT_WIDTH     80
#define TEXT_HEIGHT    25
#define TEXT_MEM_SIZE  (TEXT_WIDTH * TEXT_HEIGHT * 2) // 2 bytes per char
#define TEXT_MEM_BEGIN 0xB8000
#define TEXT_MEM_END   (TEXT_MEM_BEGIN + TEXT_MEM_END)

/** Various text color modes **/
#define KIO_WHITE_ON_BLACK      0x0F
#define KIO_WHITE_ON_WHITE      0xFF
// blink codes
// default
#define KIO_DEFAULT_COLOR       KIO_WHITE_ON_BLACK

/** Globals    **/

/** Structures **/

/** Functions  **/

/*
** Prints chars to the screen, with specific color code
**
** @param str String to print
** @param color_code Set the color code of text to draw
*/
void kio_print_color(const char* str, uint8_t color_code);

/*
** Prints chars to the screen
**
** @param str String to print
*/
void kio_print(const char* str);

/*
** Clears screen
*/
void kio_clr();

/*
** Changes color of text memory
**
** @param color_code Text foreground/background color code
*/
void kio_set_color(uint8_t color_code);

#endif
