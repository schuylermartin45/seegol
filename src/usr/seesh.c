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

#include "../kern/debug.h"
#include "../kern/kio.h"
// definitions for all programs
#include "program.h"
// user program headers
#include "hellow.h"
#include "hsc_tp.h"
#include "slidedeck.h"
#include "slideshow.h"
#include "trench_run.h"

/** Macros    **/
// number of shell commands. This is kept in the C file out of convenience
#define BUILT_IN_COUNT  3
#define PROG_COUNT      (BUILT_IN_COUNT + 5)

// structure that holds all program information
static Program prog_lst[PROG_COUNT];

/************************** Built-in Main Functions **************************/

/*
** Main method for help menu program
*/
static uint8_t _help_main(uint8_t argc, char* argv[])
{
    // help with no arguments well dump a list of commands 
    if (argc == 1)
    {
        kio_print("SeeSH- Program List ('help [program]' for more info)\n");
        kio_print("====================================================\n");
        for (uint8_t i=0; i<PROG_COUNT; ++i)
            kio_printf("%D - %s\n", &i, prog_lst[i].name);
    }
    // help with an argument will dump more information about a program
    else if (argc == 2)
    {
        for (uint8_t i=0; i<PROG_COUNT; ++i)
        {
            if (kio_strcmp(prog_lst[i].name, argv[1]))
            {
                kio_printf("Usage: %s %s\n", prog_lst[i].name,
                    prog_lst[i].usage
                );
                kio_printf("  %s\n", prog_lst[i].desc, NULL);
                break;
            }
        }
    }
    else
        return ERR_PROG_USAGE;
    return EXIT_SUCCESS;
}

/*
** Main method for help menu program
*/
static uint8_t _clear_main(uint8_t argc, char* argv[])
{
    kio_clr();
    return EXIT_SUCCESS;
}

/************************** Local Helper Functions **************************/

/*
** Parses command line arguments provided by the user by modifying the buffer.
** argv[] stores pointers to strings in the prompt buffer, delimited by spaces
**
** @param buff String buffer of arguments
** @param argv Argument array to set (program name at position 0)
** @return Arg count
*/
static uint8_t __parse_args(char* buff, char* argv[])
{
    uint16_t argc = 1;
    // first argument is the name of the program
    argv[0] = buff;
    while (*buff != '\0')
    {
        // if we find a space, replace it with a NULL byte and point to the
        // the start of the next argument
        if (*buff == ' ')
        {
            *buff++ = '\0';
            // skip over any additional whitespace, filling w/ NULLs
            while (*buff == ' ')
                *buff++ = '\0';
            argv[argc++] = buff;
        }
        else
            ++buff;
    }
    return argc;
}

/*
** Initialize the shell with program structure information
**
** @param prog_lst Array of program structures
*/
static void __init(Program* prog_lst)
{
    // built-in commands
    prog_lst->name = "exit";
    prog_lst->desc = "Bail from SeeSH. Game Over.";
    prog_lst->usage = "";
    prog_lst->main = NULL;
    ++prog_lst;
    prog_lst->name = "clear";
    prog_lst->desc = "Clears the screen.";
    prog_lst->usage = "";
    prog_lst->main = &_clear_main;
    ++prog_lst;
    prog_lst->name = "help";
    prog_lst->desc = "Help menu. Describes other programs.";
    prog_lst->usage = "[program]";
    prog_lst->main = &_help_main;
    ++prog_lst;
    // user programs
    hellow_init(prog_lst);
    ++prog_lst;
    hsc_tp_init(prog_lst);
    ++prog_lst;
    slidedeck_init(prog_lst);
    ++prog_lst;
    slideshow_init(prog_lst);
    ++prog_lst;
    trench_run_init(prog_lst);
    ++prog_lst;
}

/*
** Main execution point of the shell
*/
uint8_t seesh_main(void)
{
    kio_print(MSG_SHELL_START);
    __init(prog_lst);
    // shell control loop
    while(true)
    {
        char prompt_buff[SHELL_BUFF_SIZE];
        // TODO check this; shouldn't be necessary(?)
        // flush the buffer, otherwise wrong commands might execute
        for(uint8_t i=0; i<SHELL_BUFF_SIZE; ++i)
            prompt_buff[i] = '\0';
        kio_prompt(SHELL_PROMPT, prompt_buff);
        // special bail command
        if (kio_strcmp(prog_lst[0].name, prompt_buff))
            break;
        // programs have error codes; default to not found exception
        uint8_t err_code = ERR_PROG_NOT_FOUND;
        // parse the command line arguments
        char* argv[SHELL_ARG_SIZE];
        uint8_t argc = __parse_args(prompt_buff, argv);
        // cntr defined out here for error messaging later
        uint8_t id;
        // match the command with a program, and call it
        for (id=1; id<PROG_COUNT; ++id)
        {
            // execute program, if it is found and pass the arguments over
            if (kio_strcmp(prog_lst[id].name, argv[0]))
            {
                err_code = prog_lst[id].main(argc, argv);
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
                kio_printf("ERROR: program '%s' not found\n", argv[0], NULL);
                break;
            case ERR_PROG_USAGE:
            case ERR_PROG_BAD_ARGS:
                kio_printf("Usage: %s %s\n", argv[0], prog_lst[id].usage);
                break;
            default:
                kio_printf("ERROR: '%s' exited with code %D\n",
                    argv[0], &err_code
                );
        }
    }
    kio_print("Good-bye!\n");
    return EXIT_SUCCESS;
}
