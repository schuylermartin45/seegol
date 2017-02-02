/*
** File:    kio.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Kernel I/O for basic debugging purposes
**              This file provides the faculties to print an auxiliary buffer
**              when graphics mode is enabled
*/

/** Headers    **/
#include "gcc16.h"
#include "kio.h"

// pointer to video memory; character to display
volatile char* txt_ptr = (volatile char*)TEXT_MEM_BEGIN;

// alternative frame buffer for text output. This will allow us to write to
// text memory even in graphics mode, analogous to a TTY session on Linux
volatile char txt_fb[TEXT_MEM_SIZE];
// reference to the current start of text memory. This will depend on which
// buffer we are writing to
volatile char* txt_mem_begin = (volatile char*)TEXT_MEM_BEGIN;

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
        // encounter a newline, jump ahead to the next line
        if (*str == '\n')
        {
            // jump to the start of the next line (one line after the current)
            uint8_t ln_num = ((txt_ptr - txt_mem_begin) / TEXT_MEM_WIDTH) + 1;
            txt_ptr = txt_mem_begin + (ln_num * TEXT_MEM_WIDTH);
            // advance to next char
            str++;
        }
        else
        {
            // first byte: ASCII char
            *txt_ptr++ = *str++;
            // second byte: color code
            *txt_ptr++ = color_code;
        }
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
    txt_ptr = txt_mem_begin;
    // use 16-bit access ptr to clear memory; cuts memory access in half
    volatile uint16_t* clr_ptr = (volatile uint16_t*)txt_mem_begin;
    for(uint16_t i=0; i<(TEXT_MEM_SIZE / 2); ++i)
    {
        clr_ptr[i] = 0;
    }
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
    txt_ptr = txt_mem_begin;
    for(uint16_t i=1; i<TEXT_MEM_SIZE; i+=2)
    {
        // second byte: color code
        txt_ptr[i] = color_code;
    }
    // reset to last position
    txt_ptr = tmp_ptr;
}

