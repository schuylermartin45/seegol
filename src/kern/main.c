/*
** File:    main.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Main execution point of the OS
*/

/** Headers    **/
#include "gcc16.h"
#include "debug.h"
#include "kio.h"
#include "types.h"
// TODO rm
#include "asm_lib.h"

/*
** Main execution point of the OS
*/
void main(void)
{
    // TODO rm when kprint works
    for(uint16_t cntr=0; cntr<5; cntr++)
    {
        __asm__ __volatile__ ("int $0x10" : : "a"(0x0E00 | 'X'), "b"(7));
    }
    __put_chr('A');
    // main runtime loop
    // TODO fix
    k_print("\nKernel Loaded\n");
    while(true)
    {
    }
}
