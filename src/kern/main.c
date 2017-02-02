/*
** File:    main.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Main execution point of the OS
*/

#define MSG_KERN_LOAD   "Kernel Loaded..."
#define MSG_WELCOME     "Welcome to SeeGOL!"

/** Headers    **/
#include "gcc16.h"
#include "types.h"
#include "debug.h"
#include "kio.h"


/*
** Main execution point of the OS
*/
void main(void)
{
    // kernel print messaging
    kio_print(MSG_KERN_LOAD);
    kio_print(MSG_WELCOME);

    // primary OS control loop
    while(true)
    {
    }
}
