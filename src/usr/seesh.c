/*
** File:    seesh.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: SeeSH shell program for SeeGOL
*/

/** Headers    **/
#include "../kern/gcc16.h"
#include "seesh.h"

/** Macros    **/
#define PROG_COUNT 3

// structure that holds all program information
Program prog_lst[PROG_COUNT];

/*
** Main method for hello world program
*/
uint16_t hellow_main(uint16_t argc, char* argv)
{
    kio_print("Hello, world!\n");
    return EXIT_SUCCESS;
}

/*
** Main method for help menu program
*/
uint16_t help_main(uint16_t argc, char* argv)
{
    kio_print("SeeSH Help Menu - Available Programs:\n");
    kio_print("=====================================\n");
    for (uint8_t i=0; i<PROG_COUNT; i++)
    {
        kio_printf("%D - %s\n", &i, prog_lst[i].name);
        kio_printf("  %s\n", prog_lst[i].desc, NULL);
    }
    return EXIT_SUCCESS;
}

/*
** Initialize the shell with program structure information
*/
static void __init(Program* prog_lst)
{
    prog_lst->name = "exit";
    prog_lst->desc = "Bail from SeeSH. Game Over.";
    prog_lst->main = NULL;
    ++prog_lst;
    prog_lst->name = "help";
    prog_lst->desc = "Help menu. Describes other programs";
    prog_lst->main = &help_main;
    ++prog_lst;
    prog_lst->name = "hello_world";
    prog_lst->desc = "Basic \"Hello World\" program.";
    prog_lst->main = &hellow_main;
    ++prog_lst;
}

/*
** Main execution point of the shell
*/
uint16_t seesh_main(void)
{
    kio_print(MSG_SHELL_START);
    __init(prog_lst);
    // shell control loop
    while(true)
    {
        char prompt_buff[SHELL_BUFF_SIZE];
        kio_prompt(SHELL_PROMPT, prompt_buff);
        // special bail command
        if (kio_strcmp(prog_lst[0].name, prompt_buff))
        {
            break;
        }
        // programs have error codes
        uint8_t err_code = ERR_PROG_NOT_FOUND;
        char* err_name = prompt_buff;
        // match the command with a program, and call it
        for (uint8_t i=1; i<PROG_COUNT; i++)
        {
            // execute program, if it is found
            if (kio_strcmp(prog_lst[i].name, prompt_buff))
            {
                err_code = prog_lst[i].main(0, prompt_buff);
                err_name = prog_lst[i].name;
                break;
            }
        }
        // error messaging
        switch (err_code)
        {
            // do nothing on success
            case EXIT_SUCCESS:
                break;
            case ERR_PROG_NOT_FOUND:
                kio_printf("ERROR: program '%s' not found\n", err_name, NULL);
                break;
            default:
                kio_printf("ERROR: '%s' exited with code %D\n",
                    err_name, &err_code
                );
        }
    }
    kio_print("Good-bye!\n");
    return EXIT_SUCCESS;
}
