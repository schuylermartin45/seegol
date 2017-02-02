/*
** File:    kio.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Kernel I/O for basic debugging purposes
*/

/** Headers    **/
#include "gcc16.h"
#include "kio.h"

// pointer to video memory; character to display
volatile char* txt_ptr = (volatile char*)TEXT_MEM_BEGIN;

/*
** Kernel print: prints chars to the screen
**
** @param str String to print
*/
void k_print(const char* str)
{
    //char* str = "five";
    while(*str != 0)
    {
        // first byte: ASCII char
        *txt_ptr = *str++;
        txt_ptr++;
        // second byte: color code
        *txt_ptr = KIO_DEFAULT_COLOR;
        txt_ptr++;
    }
}

/*
** Kernel printf: k_print with some functionality of the STDIO printf
**
** @param str String to print
** @param TODO
*/
void k_printf(const char* str)
{
    k_print(str);
}
