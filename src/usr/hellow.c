/*
** File:    hellow.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Hello World demo program
*/

/** Headers    **/
#include "gcc16.h"
#include "hellow.h"

/*
** Initializes program structure
**
** @param prog Program pointer to set
*/
void hellow_init(Program* prog)
{
    prog->name = "hello_world";
    prog->desc = "Basic \"Hello World\" program.";
    prog->main = &hellow_main;
}

/*
** Main method for hello world program
*/
uint16_t hellow_main(uint16_t argc, char* argv)
{
    kio_print("Hello, world!\n");
    return EXIT_SUCCESS;
}
