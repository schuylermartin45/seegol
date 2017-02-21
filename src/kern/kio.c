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
static volatile char* txt_ptr = (volatile char*)TEXT_MEM_BEGIN;

// alternative frame buffer for text output. This will allow us to write to
// text memory even in graphics mode, analogous to a TTY session on Linux
static volatile char txt_fb[TEXT_MEM_SIZE];
// reference to the current start of text memory. This will depend on which
// buffer we are writing to
static volatile char* txt_mem_begin = (volatile char*)TEXT_MEM_BEGIN;

/************************** String Functions **************************/

/*
** Returns the digit length of a positive integer
**
** @param num Integer to print
** @param base Base representation
** @return Length of the integer
*/
static uint8_t __kio_int_len(uint16_t num, uint8_t base)
{
    uint8_t len = 0;
    // perform the conversion, which reverses the digit order
    do
    {
        num /= base;
        ++len;
    } while(num);
    return len;
}

/*
** Converts a positive integer into a string
**
** @param buff Buffer to write to
** @param num Integer to print
** @param base Base representation
** @return Length of the integer
*/
static uint8_t __kio_int_str(char* buff, uint16_t num, uint8_t base)
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
    return len;
}

/*
** Compares two strings for equivalency
**
** @param str0 First string
** @param str1 Second string
** @return True if strings are the same, false otherwise
*/
bool kio_strcmp(const char* str0, const char* str1)
{
    // check character by character, bailing if there's a mismatch
    while(*str0 == *str1)
    {
        if ((*str0 == '\0') || (*str1 == '\0'))
            break;
        ++str0; ++str1;
    }
    // report matches
    if ((*str0 == '\0') && (*str1 == '\0'))
        return true;
    else
        return false;
}

/*
** Returns the length of a string
**
** @param str String
** @return Length of the string
*/
uint16_t kio_strlen(const char* str)
{
    uint16_t cntr = 0;
    while(*str++ != 0)
    {
        ++cntr;
    }
    return cntr;
}

/*
** Supported patterns for sprintf/printf functions:
**   %b | %B - Numeric in binary      (uint16_t | uint8_t)
**   %d | %D - Numeric in decimal     (uint16_t | uint8_t)
**   %x | %X - Numeric in hexadecimal (uint16_t | uint8_t)
**   %c | %C - Character
**   %s | %S - String
*/

/*
** Gets the final length of a stringsprintf-like print function, limited to 2
** arguments
**
** @param str Formatted string to print
** @param a0 First arugment to print
** @param a1 Second arugment to print
*/
uint16_t kio_sprintf_len(const char* str, void* a0, void* a1)
{
    uint16_t len = 0;
    // argument counter
    uint8_t ai = 0;
    // loop until null byte
    while(*str != 0)
    {
        // delimeter for string formatting
        if (*str == '%')
        {
            ++str;
            // assume arguments are numbers out of convience
            uint16_t* ptr = (ai == 0) ? (uint16_t*)a0 : (uint16_t*)a1;
            uint16_t val = *ptr;
            switch (*str)
            {
                // binary and hex numbers begin with 2 chars, so take that into
                // account here (i.e. '0x' and '0b')
                // binary numbers
                case 'b':
                    len += __kio_int_len(val, 2) + 2;
                    break;
                // upper case variants handle 8-bit numbers
                case 'B':
                {
                    uint8_t* ptr = (ai == 0) ? (uint8_t*)a0 : (uint8_t*)a1;
                    uint8_t val = *ptr;
                    len += __kio_int_len(val, 2) + 2;
                    break;
                }
                // decimal numbers
                case 'd':
                    len += __kio_int_len(val, 10);
                    break;
                case 'D':
                {
                    uint8_t* ptr = (ai == 0) ? (uint8_t*)a0 : (uint8_t*)a1;
                    uint8_t val = *ptr;
                    len += __kio_int_len(val, 10);
                    break;
                }
                // hex numbers
                case 'x':
                    len += __kio_int_len(val, 16) + 2;
                    break;
                case 'X':
                {
                    uint8_t* ptr = (ai == 0) ? (uint8_t*)a0 : (uint8_t*)a1;
                    uint8_t val = *ptr;
                    len += __kio_int_len(val, 16) + 2;
                    break;
                }
                // char/string
                case 'c': case 'C':
                    ++len;
                    break;
                case 's': case 'S':
                    if (ai == 0)
                        len += kio_strlen((char*)a0);
                    else
                        len += kio_strlen((char*)a1);
                    break;
            }
            ++str;
            ++ai;
        }
        else
        {
            ++str;
            ++len;
        }
    }
    // and one for the null byte; just to be safe
    return len + 1;
}

/*
** sprintf-like print function, limited to 2 arguments
**
** @param str String to print
** @param buff Buffer to put finalized string in
** @param a0 First arugment to print
** @param a1 Second arugment to print
*/
void kio_sprintf(const char* str, char* buff, void* a0, void* a1)
{
    // argument counter
    uint8_t ai = 0;
    // loop until null byte
    while(*str != 0)
    {
        // delimeter for string formatting
        if (*str == '%')
        {
            ++str;
            // buffer for printf text processing
            char p_buff[TEXT_WIDTH];
            // assume arguments are numbers out of convience
            uint16_t* ptr = (ai == 0) ? (uint16_t*)a0 : (uint16_t*)a1;
            uint16_t val = *ptr;
            // determines with argument value to draw
            char* arg_str = p_buff;
            switch (*str)
            {
                // binary numbers
                case 'b':
                    __kio_int_str(p_buff, val, 2);
                    break;
                // upper case variants handle 8-bit numbers
                case 'B':
                {
                    uint8_t* ptr = (ai == 0) ? (uint8_t*)a0 : (uint8_t*)a1;
                    uint8_t val = *ptr;
                    __kio_int_str(p_buff, val, 2);
                    break;
                }
                // decimal numbers
                case 'd':
                    __kio_int_str(p_buff, val, 10);
                    break;
                case 'D':
                {
                    uint8_t* ptr = (ai == 0) ? (uint8_t*)a0 : (uint8_t*)a1;
                    uint8_t val = *ptr;
                    __kio_int_str(p_buff, val, 10);
                    break;
                }
                // hex numbers
                case 'x':
                    __kio_int_str(p_buff, val, 16);
                    break;
                case 'X':
                {
                    uint8_t* ptr = (ai == 0) ? (uint8_t*)a0 : (uint8_t*)a1;
                    uint8_t val = *ptr;
                    __kio_int_str(p_buff, val, 16);
                    break;
                }
                // char/string
                case 'c': case 'C':
                    arg_str = p_buff;
                    p_buff[0] = (ai == 0) ? *((char*)a0) : *((char*)a1);
                    p_buff[1] = '\0';
                    break;
                case 's': case 'S':
                    arg_str = (ai == 0) ? (char*)a0 : (char*)a1;
                    break;
            }
            // print the value of the argument
            while(*arg_str != 0)
            {
                *buff++ = *arg_str++;
            }
            ++str;
            ++ai;
        }
        else
        {
            *buff++ = *str++;
        }
    }
    // null pad
    *buff = '\0';
}

/************************** Output Functions **************************/

/*
** Check if text memory is used up; scroll all of text memory up one line
*/
static void __kio_chk_scroll()
{
    // ran out of space, scroll up; otherwise don't run this method
    if (txt_ptr < (txt_mem_begin + TEXT_MEM_SIZE))
        return;
    // use 16-bit access ptr to copy memory; cuts memory access in half
    volatile uint16_t* cp_ptr = (volatile uint16_t*)txt_mem_begin;
    for(uint16_t i=0; i<(TEXT_SIZE - TEXT_WIDTH); ++i)
        cp_ptr[i] = cp_ptr[i + TEXT_WIDTH];
    // clear last line
    cp_ptr = (volatile uint16_t*)txt_mem_begin;
    cp_ptr += TEXT_SIZE - TEXT_WIDTH;
    for(uint16_t i=0; i<TEXT_WIDTH; ++i)
        cp_ptr[i] = 0;
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
** printf-like print function, limited to 2 arguments
**
** @param str String to print
** @param color_code Set the color code of text to draw
** @param a0 First arugment to print
** @param a1 Second arugment to print
*/
void kio_printf_color(const char* str, uint8_t color_code, void* a0, void* a1)
{
    // autosize buffer; first case of "dynamic memory" anywhere in the OS!
    // Go me! WOOT!
    uint16_t size = kio_sprintf_len(str, a0, a1);
    char buff[size];
    // call sprintf, then dump to the screen
    kio_sprintf(str, buff, a0, a1);
    kio_print_color(buff, color_code);
}

/*
** printf-like print function, limited to 2 arguments
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
** Clears current screen
*/
void kio_clr()
{
    // reset text to 1st position
    txt_ptr = txt_mem_begin;
    // use 16-bit access ptr to clear memory; cuts memory access in half
    volatile uint16_t* clr_ptr = (volatile uint16_t*)txt_mem_begin;
    for(uint16_t i=0; i<TEXT_SIZE; ++i)
        clr_ptr[i] = 0;
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
    // odd addresses; change second byte/color code only
    for(uint16_t i=1; i<TEXT_MEM_SIZE; i+=2)
        txt_ptr[i] = color_code;
    // reset to last position
    txt_ptr = tmp_ptr;
}

/*
** Swap currently active text frame buffer
*/
void kio_swap_fb()
{
    // use 16-bit ptrs to cut memory access in half
    volatile uint16_t* cp_mem_ptr = (volatile uint16_t*)TEXT_MEM_BEGIN;
    volatile uint16_t* cp_fb_ptr = (volatile uint16_t*)txt_fb;
    // frame buffer is currently active
    if (txt_mem_begin == txt_fb)
    {
        // update pointers to relative addresses in the video memory
        txt_ptr = (volatile char*)TEXT_MEM_BEGIN + (txt_ptr - txt_mem_begin);
        txt_mem_begin = (volatile char*)TEXT_MEM_BEGIN;
        // copy current video memory context to the frame buffer
        for(uint16_t i=0; i<TEXT_SIZE; ++i)
            cp_mem_ptr[i] = cp_fb_ptr[i];
    }
    // video memory is currently active
    else
    {
        // copy current video memory context to the frame buffer
        for(uint16_t i=0; i<TEXT_SIZE; ++i)
        {
            cp_fb_ptr[i] = cp_mem_ptr[i];
            // clear text memory post-copy; prevents artifacts in graphics mode
            cp_mem_ptr[i] = 0;
        }
        // update pointers to relative addresses in the frame buffer
        txt_ptr = txt_fb + (txt_ptr - txt_mem_begin);
        txt_mem_begin = txt_fb;
    }
}

/************************** Input Functions **************************/

/*
** Fetches a single char from the user
**
** @return Character from the user
*/
char kio_getchr()
{
    // BIOS interrupt to get a character
    __asm__ __volatile__("movb $0, %ah\n");
    __asm__ __volatile__("int $0x16\n");
    // %al has the character
    char ch;
    __asm__ __volatile__("movb %%al, %0\n" : "=rm"(ch));
    return ch;
}

/*
** Blocking wait that waits for a single char from the user
**
** @param Character from the user
*/
void kio_wait_key(char ch)
{
    while (kio_getchr() != ch) {}
}

/*
** Fetches a null-terminated string (ended with a newline) from the user
**
** @param str String buffer to put chars into
*/
void kio_getstr(char* str)
{
    char ch;
    while(true)
    {
        ch = kio_getchr();
        // enter key on my keyboard is being interpretted as a carriage return
        if ((ch == '\r') || (ch == '\n'))
        {
            *str++ = '\0';
            break;
        }
        else
            *str++ = ch;
    }
}

/*
** Fetches a null-terminated string (ended with a newline) from the user
** with some higher level functionality, drawing to the screen and able to
** handle backspaces. Similar to Python's `input()` function
**
** @param prompt Prompt to show before string input
** @param color_code Set the color code of text to draw
** @param str String buffer to put chars into
*/
void kio_prompt_color(char* prompt, uint8_t color_code, char* str)
{
    // dump prompt to the screen
    kio_print_color(prompt, color_code);
    // record the prompt mem location
    char* prompt_loc = (char*)txt_ptr;
    // fetch chars until newline
    char ch;
    while(true)
    {
        ch = kio_getchr();
        switch (ch)
        {
            // if it's a backspace, move back once in video memory and wipe
            case ASCII_BACKSPACE:
            case ASCII_DELETE:
                // prevent deleting the prompt and prior text from video memory
                if ((txt_ptr - prompt_loc) > 0)
                {
                    // go back 2 bytes (char and color code), write a blank
                    // char, and then go back again so that the next char will
                    // write on top of the blank character
                    txt_ptr -= 2;
                    kio_print_color(" ", color_code);
                    txt_ptr -= 2;
                    // also wipe the string storage 1 character
                    --str;
                }
                break;
            // terminate string on enter key
            case '\r':
            case '\n':
                // terminate string, add newline after prompt, and bail
                *str = '\0';
                kio_print("\n");
                return;
            // Some keys don't have 8-bit ASCII codes, but rather 16 bit key
            // codes, the lower 8-bits being the null byte. These include the
            // arrow keys. For now, instead of adding history to the prompt, we
            // will just dump a dummy character. This at least allows the user
            // to correct the mistake; before no command could be entered
            case '\0':
                ch = '^';
            // otherwise, draw character
            default:
                *str++ = ch;
                kio_printf_color("%c", color_code, &ch, NULL);
        }
    }
}

/*
** Fetches a null-terminated string (ended with a newline) from the user
** with some higher level functionality, drawing to the screen (similar to
** Python's `input()` function)
**
** @param prompt Prompt to show before string input
** @param str String buffer to put chars into
*/
void kio_prompt(char* prompt, char* str)
{
    kio_prompt_color(prompt, KIO_DEFAULT_COLOR, str);
}
