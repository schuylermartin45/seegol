/*
** File:    main.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Main execution point of the OS
*/

/** Prep addressing **/
#include "gcc16.h"
//__asm__("jmpl $0x0000, $main\n");

/** Headers    **/
#include "debug.h"
#include "kio.h"
#include "types.h"

// make main accessible to the bootloader, boot.s
extern uint16_t main(void);

/*
** Main execution point of the OS
*/
uint16_t main(void)
{
    for(uint16_t cntr=0; cntr<5; cntr++)
    {
        __asm__ __volatile__ ("int $0x10" : : "a"(0x0E00 | 'Y'), "b"(7));
    }
    return 0;
    // main runtime loop
    /* TODO fix
    k_print("Kernel Loaded");
    while(true)
    {
    }
    */
}
