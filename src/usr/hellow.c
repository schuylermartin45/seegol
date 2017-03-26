/*
** File:    hellow.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Hello World demo program
*/

/** Headers    **/
#include "../kern/gcc16.h"
#include "hellow.h"

#include "../kern/kio.h"

/*
** Initializes program structure
**
** @param prog Program pointer to set
*/
void hellow_init(Program* prog)
{
    prog->name = "hello_world";
    prog->desc = "Basic \"Hello World\" program.";
    prog->usage = "";
    prog->main = &hellow_main;
}

/*
** Main method for the program
*/
uint8_t hellow_main(uint8_t argc, char* argv[])
{
    kio_print("Hello, world!\n");
    return EXIT_SUCCESS;
}
