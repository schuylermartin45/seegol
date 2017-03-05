/*
** File:    trench_run.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: These plans are NOT in the main computer
*/

/** Headers    **/
#include "../kern/gcc16.h"
#include "trench_run.h"

#include "../kern/kio.h"
#include "../gl/gl_lib.h"

/*
** Initializes program structure
**
** @param prog Program pointer to set
*/
void trench_run_init(Program* prog)
{
    prog->name = "trench_run";
    prog->desc = "These plans are NOT in the main computer.";
    prog->usage = "";
    prog->main = &trench_run_main;
}

/*
** Main method for program
*/
uint8_t trench_run_main(uint8_t argc, char* argv[])
{
    gl_init();
    // default to VGA13
    gl_enter(VGA_MODE_13);

    // draw the plans
    gl_draw_line(PT2(0,0), PT2(50,50), RGB_WHITE);
    kio_wait_key('q');

    // clean-up and exit
    gl_exit();
    return EXIT_SUCCESS;
}
