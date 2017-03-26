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
#include "usr_clock.h"

/** Macros    **/
// number of shell commands. This is kept in the C file out of convenience
#define BUILTIN_COUNT   3
#define PROG_COUNT      (BUILTIN_COUNT + 6)
// macros for installing programs to SeeSH
#define INSTALL_BUILTIN_PROG(n, d, u, m) \
    prog_lst->name = n;\
    prog_lst->desc = d;\
    prog_lst->usage = u;\
    prog_lst->main = m;\
    ++prog_lst;
#define INSTALL_USR_PROG(init)      init(prog_lst); ++prog_lst;

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
        bool prog_found = false;
        for (uint8_t i=0; i<PROG_COUNT; ++i)
        {
            if (kio_strcmp(prog_lst[i].name, argv[1]))
            {
                kio_printf("Usage: %s %s\n", prog_lst[i].name,
                    prog_lst[i].usage
                );
                kio_printf("  %s\n", prog_lst[i].desc, NULL);
                prog_found = true;
                break;
            }
        }
        if (!prog_found)
        {
            kio_printf("Program '%s' not found.\n", argv[1], NULL);
            return EXIT_FAILURE;
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
    kio_clrscr();
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
    INSTALL_BUILTIN_PROG("exit", "Bail from SeeSH", "", NULL);
    INSTALL_BUILTIN_PROG("clear", "Clears the screen.", "", &_clear_main);
    INSTALL_BUILTIN_PROG("help", "Help menu. Describes other programs.",
        "[program]", &_help_main);
    // user programs
    INSTALL_USR_PROG(hellow_init);
    INSTALL_USR_PROG(hsc_tp_init);
    INSTALL_USR_PROG(slidedeck_init);
    INSTALL_USR_PROG(slideshow_init);
    INSTALL_USR_PROG(trench_run_init);
    INSTALL_USR_PROG(usr_clock_init);
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
