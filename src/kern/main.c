/*
** File:    main.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Main execution point of the OS
*/

/** Prep addressing **/
#include "gcc16.h"
__asm__("jmpl $0x0000, $main\n");

/** Headers    **/
#include "debug.h"
#include "types.h"

/*
** Main execution point of the OS
*/
uint16_t main(void)
{
    // main runtime loop
    while(true)
    {
    }
	return(0);
}
