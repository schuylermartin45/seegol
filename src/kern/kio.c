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

/*
** Kernel print: prints chars to the screen
**
** @param str String to print
*/
void k_print(const char *str)
{
    while(*str)
    {
        __put_chr(*str);
        str++;
    }
}

/*
** Kernel printf: k_print with some functionality of the STDIO printf
**
** @param str String to print
** @param TODO
*/
void k_printf(const char *str)
{
    while(*str)
    {
        __put_chr(*str);
        str++;
    }
}
