/*
** File:    main.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Main execution point of the OS
*/

/** Headers    **/
#include "gcc16.h"
#include "kio.h"
#include "pit.h"

#include "../usr/seesh.h"

// main welcome messages
#define MSG_KERN_LOAD   "Kernel Loaded...\n"
#define MSG_WELCOME     "Welcome to SeeGOL!\n"
#define MSG_KERN_EXIT   "< Kernel Exited >\n"

/*
** Main execution point of the OS
*/
void main(void)
{
    // Hardware initialization
    pit_init();

    // kernel print messaging
    kio_print(MSG_KERN_LOAD);
    kio_print(MSG_WELCOME);

    // primary OS control loop
    while(true)
    {
        // call the seesh shell to run commands
        seesh_main();
        kio_printf("System clock value: %d\n", &pit_clock_value, NULL);
    }
    kio_print(MSG_KERN_EXIT);
}
