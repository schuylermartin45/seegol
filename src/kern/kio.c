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
** Prints chars to the screen, with specific color code
**
** @param str String to print
** @param color_code Set the color code of text to draw
*/
void kio_print_color(const char* str, uint8_t color_code)
{
    // loop until null byte
    while(*str != 0)
    {
        // first byte: ASCII char
        *txt_ptr++ = *str++;
        // second byte: color code
        *txt_ptr++ = color_code;
    }
}

/*
** Prints chars to the screen
**
** @param str String to print
*/
void kio_print(const char* str)
{
    kio_print_color(str, KIO_DEFAULT_COLOR);
}

/*
** Clears screen
*/
void kio_clr()
{
    // reset text to 1st position
    txt_ptr = (volatile char*)TEXT_MEM_BEGIN;
    //char* str = "five";
    for(uint16_t i=0; i<TEXT_MEM_SIZE; ++i)
    {
        txt_ptr[i] = 0;
    }
    // reset cursor again
    txt_ptr = (volatile char*)TEXT_MEM_BEGIN;
}

/*
** Changes color of text memory
**
** @param color_code Text foreground/background color code
*/
void kio_set_color(uint8_t color_code)
{
    volatile char* tmp_ptr = txt_ptr;
    // reset text to 1st position
    txt_ptr = (volatile char*)TEXT_MEM_BEGIN;
    // loop until null byte
    for(uint16_t i=1; i<TEXT_MEM_SIZE; i+=2)
    {
        // second byte: color code
        txt_ptr[i] = color_code;
    }
    // reset to last position
    txt_ptr = tmp_ptr;
}

