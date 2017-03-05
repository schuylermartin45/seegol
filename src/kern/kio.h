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
// dimensions in terms of characters
#define TEXT_WIDTH          80
#define TEXT_HEIGHT         25
#define TEXT_SIZE           (TEXT_WIDTH * TEXT_HEIGHT)
// MEM indicates we're talking in terms of bytes (2 bytes per char)
#define TEXT_MEM_WIDTH      (TEXT_WIDTH * 2)
#define TEXT_MEM_HEIGHT     TEXT_HEIGHT
#define TEXT_MEM_SIZE       (TEXT_WIDTH * TEXT_HEIGHT * 2)
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

// common key codes
#define KEY_ESC     27
#define KEY_SPACE   32

/** Globals    **/

/** Structures **/

/** Functions  **/

/************************** String Functions **************************/

/*
** Converts a string representation of a positive integer to an integer
** assumes that bases > 10 use capital letters (following the ASCII chart)
** and that input only uses valid digits (this may cause numeric overflow)
**
** @param buff Buffer to write to
** @param base Base representation
** @return Integer representation
*/
uint16_t kio_str_int(char* buff, uint16_t base);

/*
** Compares two strings for equivalency
**
** @param str0 First string
** @param str1 Second string
** @return True if strings are the same, false otherwise
*/
bool kio_strcmp(const char* str0, const char* str1);

/*
** Returns the length of a string
**
** @param str String
** @return Length of the string
*/
uint16_t kio_strlen(const char* str);

/*
** Supported patterns for sprintf/printf functions:
**   %b | %B - Numeric in binary      (uint16_t | uint8_t)
**   %d | %D - Numeric in decimal     (uint16_t | uint8_t)
**   %x | %X - Numeric in hexadecimal (uint16_t | uint8_t)
**   %c | %C - Character
**   %s | %S - String
**
** Putting a single digit in front of the type will left-pad if the argument
** is shorter than the padding. Example: "|%5d|%5d|"
**   |   12|123456|
** This feature can also pad with 0s. Example: "|%05d|%05d|"
**   |00012|123456|
*/

/*
** Gets the final length of a stringsprintf-like print function, limited to 2
** arguments
**
** @param str Formatted string to print
** @param a0 First arugment to print
** @param a1 Second arugment to print
*/
uint16_t kio_sprintf_len(const char* str, void* a0, void* a1);

/*
** sprintf-like print function, limited to 2 arguments
**
** @param str String to print
** @param buff Buffer to put finalized string in
** @param a0 First arugment to print
** @param a1 Second arugment to print
*/
void kio_sprintf(const char* str, char* buff, void* a0, void* a1);

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
** printf-like print function, limited to 2 arguments
**
** @param str String to print
** @param color_code Set the color code of text to draw
** @param a0 First arugment to print
** @param a1 Second arugment to print
*/
void kio_printf_color(const char* str, uint8_t color_code, void* a0, void* a1);

/*
** printf-like print function, limited to 2 arguments
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
** Blocking wait that waits for a single char from the user
**
** @param Character from the user
*/
void kio_wait_key(char ch);

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

#endif
