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
#define TEXT_WIDTH          80
#define TEXT_HEIGHT         25
#define TEXT_SIZE           (TEXT_WIDTH * TEXT_HEIGHT)
#define TEXT_MEM_WIDTH      (TEXT_WIDTH * 2) // 2 bytes per char
#define TEXT_MEM_HEIGHT     TEXT_HEIGHT
#define TEXT_MEM_SIZE       (TEXT_WIDTH * TEXT_HEIGHT * 2) // 2 bytes per char
#define TEXT_MEM_BEGIN      0xB8000
#define TEXT_MEM_END        (TEXT_MEM_BEGIN + TEXT_MEM_END)

// Some ASCII character definitions
#define ASCII_BACKSPACE     8
#define ASCII_DELETE        127

/** Various text color modes **/
#define KIO_WHITE_ON_BLACK      0x0F
#define KIO_WHITE_ON_WHITE      0xFF
// blink codes
// default
#define KIO_DEFAULT_COLOR       KIO_WHITE_ON_BLACK

/** Globals    **/

/** Structures **/

/** Functions  **/

/************************** Output Functions **************************/

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
** Printf-like print function, limited to 2 arguments
**   Supported patterns:
**     %b | %B - Numeric in binary      (uint16_t | uint8_t)
**     %d | %D - Numeric in decimal     (uint16_t | uint8_t)
**     %x | %X - Numeric in hexadecimal (uint16_t | uint8_t)
**     %c | %C - Character
**     %s | %S - String
**
** @param str String to print
** @param color_code Set the color code of text to draw
** @param a0 First arugment to print
** @param a1 Second arugment to print
*/
void kio_printf_color(const char* str, uint8_t color_code, void* a0, void* a1);

/*
** Printf-like print function, limited to 2 arguments
**   Supported patterns:
**     %b | %B - Numeric in binary      (uint16_t | uint8_t)
**     %d | %D - Numeric in decimal     (uint16_t | uint8_t)
**     %x | %X - Numeric in hexadecimal (uint16_t | uint8_t)
**     %c | %C - Character
**     %s | %S - String
**
** @param str String to print
** @param a0 First arugment to print
** @param a1 Second arugment to print
*/
void kio_printf(const char* str, void* a0, void* a1);

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

/*
** Swap currently active text frame buffer
*/
void kio_swap_fb();

/************************** Input Functions **************************/

/*
** Fetches a single char from the user
**
** @return Character from the user
*/
char kio_getchr();

/*
** Fetches a null-terminated string (ended with a newline) from the user
**
** @param str String buffer to put chars into
*/
void kio_getstr(char* str);

/*
** Fetches a null-terminated string (ended with a newline) from the user
** with some higher level functionality, drawing to the screen and able to
** handle backspaces. Similar to Python's `input()` function
**
** @param prompt Prompt to show before string input
** @param color_code Set the color code of text to draw
** @param str String buffer to put chars into
*/
void kio_prompt_color(char* prompt, uint8_t color_code, char* str);

/*
** Fetches a null-terminated string (ended with a newline) from the user
** with some higher level functionality, drawing to the screen (similar to
** Python's `input()` function)
**
** @param prompt Prompt to show before string input
** @param str String buffer to put chars into
*/
void kio_prompt(char* prompt, char* str);

/*
** Compares two strings for equivalency
**
** @param str0 First string
** @param str1 Second string
** @return True if strings are the same, false otherwise
*/
bool kio_strcmp(char* str0, char* str1);

#endif
