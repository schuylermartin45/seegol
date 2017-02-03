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
** Converts a positive integer into a string
**
** @param buff Buffer to write to
** @param num Integer to print
*/
static void __kio_int_str(char* buff, uint16_t num, uint8_t base)
{
    uint8_t len = 0;
    // hex and binary get special prefixes
    if ((base == 16) || (base == 2))
    {
        *buff++ = '0';
        *buff++ = (base == 16) ? 'x' : 'b';
    }
    // mark the beginning of the buffer when we have to reverse the string
    char* str = buff;
    // perform the conversion, which reverses the digit order
    do
    {
        uint8_t mod = num % base;
        if (mod > 9)
            *buff++ = (mod - 10) + 'A';
        else
            *buff++ = mod + '0';
        num /= base;
        ++len;
    } while(num);
    *buff = '\0';
    // reverse the string back
    uint8_t start = 0;
    uint8_t end = len - 1;
    while (start < end)
    {
        // swap beginning and end
        char tmp = *(str + start);
        *(str + start) = *(str + end);
        *(str + end) = tmp;
        // move towards the center
        ++start;
        --end;
    }
}

/*
** Check if text memory is used up; scroll all of text memory up one line
*/
static void __kio_chk_scroll()
{
    // ran out of space, scroll up
    if (txt_ptr < (txt_mem_begin + TEXT_MEM_SIZE))
        return;
    // use 16-bit access ptr to clear memory; cuts memory access in half
    volatile uint16_t* cp_ptr = (volatile uint16_t*)txt_mem_begin;
    for(uint16_t i=0; i<(TEXT_SIZE - TEXT_WIDTH); ++i)
    {
        cp_ptr[i] = cp_ptr[i + TEXT_WIDTH];
    }
    // move cursor to the start of the last line
    txt_ptr = txt_mem_begin + (TEXT_MEM_SIZE - TEXT_MEM_WIDTH);
}

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
             __kio_chk_scroll();
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
** Printf-like print function, limited to 2 arguments
**
** @param str String to print
** @param color_code Set the color code of text to draw
** @param a0 First arugment to print
** @param a1 Second arugment to print
*/
void kio_printf_color(const char* str, uint8_t color_code, void* a0, void* a1)
{
    // argument counter
    uint8_t ai = 0;
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
            ++str;
        }
        // delimeter for string formatting
        else if (*str == '%')
        {
            ++str;
            // buffer for printf text processing
            char buff[TEXT_WIDTH];
            // assume arguments are numbers out of convience
            uint16_t* ptr = (ai == 0) ? (uint16_t*)a0 : (uint16_t*)a1;
            uint16_t val = *ptr;
            // determines with argument value to draw
            char* arg_str = buff;
            switch (*str)
            {
                // binary numbers
                case 'b': case 'B':
                {
                    __kio_int_str(buff, val, 2);
                    break;
                }
                // decimal numbers
                case 'd': case 'D':
                {
                    __kio_int_str(buff, val, 10);
                    break;
                }
                // hex numbers
                case 'x': case 'X':
                {
                    __kio_int_str(buff, val, 16);
                    break;
                }
                // char/string
                case 'c': case 'C':
                    arg_str = buff;
                    buff[0] = (ai == 0) ? *((char*)a0) : *((char*)a1);
                    buff[1] = '\0';
                    break;
                case 's': case 'S':
                {
                    arg_str = (ai == 0) ? (char*)a0 : (char*)a1;
                    break;
                }
            }
            // print the value of the argument
            while(*arg_str != 0)
            {
                __kio_chk_scroll();
                *txt_ptr++ = *arg_str++;
                *txt_ptr++ = color_code;
            }
            ++str;
            ++ai;
        }
        else
        {
            __kio_chk_scroll();
            *txt_ptr++ = *str++;
            *txt_ptr++ = color_code;
        }
    }
}

/*
** Printf-like print function, limited to 2 arguments
**
** @param str String to print
** @param a0 First arugment to print
** @param a1 Second arugment to print
*/
void kio_printf(const char* str, void* a0, void* a1)
{
    kio_printf_color(str, KIO_DEFAULT_COLOR, a0, a1);
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
    for(uint16_t i=0; i<(TEXT_SIZE); ++i)
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

