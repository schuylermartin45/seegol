/*
** File:    main.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Main execution point of the OS
*/

/** Headers    **/
#include "gcc16.h"
#include "asm_lib.h"
#include "clock.h"
#include "kio.h"

#include "../usr/seesh.h"

// main welcome messages
#define MSG_KERN_LOAD   "Kernel Loaded...\n"
#define MSG_WELCOME     "Welcome to SeeGOL!\n"
#define MSG_KERN_EXIT   "< Kernel Exited >\n"

#define KEYBOARD_CMD_REGISTER   0x64
#define KEYBOARD_RESET_SIGNAL   0xFE

/*
** Reboot the system using the 8042 "keyboard" method
*/
static void __reboot()
{
    // wait for the keyboard input buffer to be empty
    uint8_t is_empty = 0x02;
    while (is_empty & 0x02)
        is_empty = _inb(KEYBOARD_CMD_REGISTER);
    // then send the CPU reset command
    _outb(KEYBOARD_CMD_REGISTER, KEYBOARD_RESET_SIGNAL);
}

/*
** Main execution point of the OS
*/
void main(void)
{
    // kernel print messaging
    kio_print(MSG_KERN_LOAD);
    kio_print(MSG_WELCOME);

    uint8_t sys_code = 0;
    // primary OS control loop
    while((sys_code != SYSTEM_SHUTDOWN) && (sys_code != SYSTEM_REBOOT))
    {
        // call the seesh shell to run commands
        sys_code = seesh_main();
    }
    // Goodbye!
    kio_print(MSG_KERN_EXIT);
    // delay so you can actually tell something happened
    clk_busy_wait(2);
    switch (sys_code)
    {
        case SYSTEM_REBOOT:
            __reboot();
    }
    // don't forget to catch fire
    __asm__ __volatile__("hlt\n");
}
