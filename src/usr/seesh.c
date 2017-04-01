/*
** File:    seesh.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: SeeSH shell program for SeeGOL. This has since been expanded to
**              include 
*/

/** Headers    **/
#include "../kern/gcc16.h"
#include "seesh.h"

#include "../kern/clock.h"
#include "../kern/kio.h"
#include "../gl/gl_lib.h"
#include "../gl/img_fids.h"
#include "../gl/pane.h"
// definitions for all programs
#include "program.h"

// user program headers
#include "hsc_tp.h"
#include "slidedeck.h"
#include "slideshow.h"
#include "trench_run.h"
#include "usr_clock.h"

/** Macros    **/
// number of programs, in various categories
#define BUILTIN_COUNT   5
#define PROG_COUNT      (BUILTIN_COUNT + 5)
// number of programs that can be launched from the GUI
#define GUI_PROG_COUNT  7
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
// partial program listing used for gui menu
static Program* gui_prog_lst;
// display names for GUI programs in the GUI menu
char* gui_prog_disp[GUI_PROG_COUNT];

/************************** Built-in Main Functions **************************/

/*
** Main method for help menu program. Handles both GUI and Text forms
*/
static uint8_t _help_main(uint8_t argc, char* argv[])
{
    // GUI help menu
    if ((argc == 1) && (argv == NULL))
    {
        pane_enter(VGA_MODE_13);
        uint8_t pid = pane_draw_prompt("Help Menu", GUI_PROG_COUNT,
            gui_prog_disp);
        pane_draw_title_text(gui_prog_lst[pid].name, gui_prog_lst[pid].desc);
        kio_wait_key('q');
        pane_exit();
    }
    // The rest of this controls the text version of the help menu
    // help with no arguments well dump a list of commands 
    else if (argc == 1)
    {
        kio_print("Help Menu - 'help [program]' for more info\n\n");
        for (uint8_t i=0; i<PROG_COUNT; ++i)
            kio_printf("%D - %s\n", &i, prog_lst[i].name);
    }
    // help with an argument will dump more information about a program
    else if (argc == 2)
    {
        bool prog_found = false;
        for (uint8_t i=0; i<PROG_COUNT; ++i)
        {
            if (kio_strcmp_case(prog_lst[i].name, argv[1], false))
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

/*
** Wrapper function that allows the GUI or SeeSH to launch itself
*/
static uint8_t _seesh_main(uint8_t argc, char* argv[])
{
    return seesh_main();
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
** Initialize the shell with program structure information. Attempts some
** protection from adding to the list, which will overflow the buffer
**
** @param prog_lst Array of program structs; passed-in to protect 1st address
*/
static void __seesh_init(Program* prog_lst)
{
    // cheap way to prevent re-initialization of the list
    if (!kio_strcmp_case(prog_lst[0].name, "exit", false))
    {
        // standard built-in commands
        INSTALL_BUILTIN_PROG("exit", "Bail from SeeSH", "", NULL);
        INSTALL_BUILTIN_PROG("clear", "Clears the screen.", "", &_clear_main);
        // user programs
        INSTALL_USR_PROG(hsc_tp_init);
        /** programs below this line will be used in the GUI display menu **/
        gui_prog_lst = prog_lst;
        INSTALL_USR_PROG(usr_clock_init);
        INSTALL_USR_PROG(slidedeck_init);
        INSTALL_USR_PROG(slideshow_init);
        INSTALL_USR_PROG(trench_run_init);
        // built-ins that can also be called from the GUI
        INSTALL_BUILTIN_PROG("Help", "Help menu. Describes other programs.",
            "[program]", &_help_main);
        INSTALL_BUILTIN_PROG("SeeSH", "SeeGOL's Shell. All programs can be run"
            "from here. Case insensitive.", "", &_seesh_main);
        INSTALL_BUILTIN_PROG("Reboot", "Reboot the system.", "", NULL);
        // namesoptions are a partial list of programs; running the shell will
        // actually deal with this
        for(uint8_t i=0; i<GUI_PROG_COUNT; ++i)
            gui_prog_disp[i] = gui_prog_lst[i].name;
    }
}

/*
** Main execution point of the shell
**
** @return exit code
*/
uint8_t seesh_main(void)
{
    kio_clrscr();
    kio_print(MSG_SHELL_START);
    __seesh_init(prog_lst);
    // exit code for the shell
    uint8_t seesh_code = EXIT_SUCCESS;
    // shell control loop
    while(true)
    {
        char prompt_buff[SHELL_BUFF_SIZE];
        // flush the buffer, otherwise wrong commands might execute
        for(uint8_t i=0; i<SHELL_BUFF_SIZE; ++i)
            prompt_buff[i] = '\0';
        kio_prompt(SHELL_PROMPT, prompt_buff);
        // special bail commands: exit, reboot, shutdown
        // reboot and shutdown are handled by the kernel's main
        if (kio_strcmp_case(prog_lst[0].name, prompt_buff, false))
            break;
        else if (kio_strcmp_case(prog_lst[PROG_COUNT - 1].name, prompt_buff,
            false))
        {
            seesh_code = SYSTEM_REBOOT;
            break;
        }
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
            if (kio_strcmp_case(prog_lst[id].name, argv[0], false))
            {
                err_code = prog_lst[id].main(argc, argv);
                // pass the buck from previous SeeSH calls
                if ((id == (PROG_COUNT - 2)) && (err_code == SYSTEM_REBOOT))
                    return err_code;
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
                kio_printf("ERROR: program '%s' not found. Type 'help' for a "
                    "complete list.\n",
                    argv[0], NULL);
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
    kio_print("SeeSH: Good-bye!\n");
    return seesh_code;
}

/***** GUI Shell *****/

/*
** System boot splash screen, like the good ol' days
*/
void seesh_splash(void)
{
    // default to VGA13
    gl_enter(VGA_MODE_13);

    gl_draw_rect_wh(PT2(0, 0), gl_getw(), gl_geth(), RGB_WHITE);
    gl_draw_img_center_scale(IMG_FID_HSC, 3);
    Point_2D str_ul = {0, 0};
    Point_2D str_bb;
    gl_draw_str_bb(str_ul, "SeeGOL", 2, gl_getw(), &str_bb);
    str_ul.x = (gl_getw() - str_bb.x) / 2;
    str_ul.y = gl_geth() - (str_bb.y + (str_bb.y / 2));
    gl_draw_str_scale(str_ul, RGB_HSC, RGB_HSC, "SeeGOL", 2, gl_getw());
    // "It really gets [the crowd excited] when they see this ramp just slowly 
    // extending down." - Rick Sanchez
    clk_busy_wait(4);
    gl_clrscr();
}

/*
** Main execution point of the "visual shell". This has limited access to
** programs on SeeGOL. SeeSH can be launched from this GUI.
**
** @return exit code
*/
uint8_t seesh_gui_main(void)
{
    __seesh_init(prog_lst);

    while(true)
    {
        // default to VGA13
        pane_enter(VGA_MODE_13);
        // user picks from the list of programs
        uint8_t pid = pane_draw_prompt("SeeGOL Programs", GUI_PROG_COUNT,
            gui_prog_disp);

        // check for reboot command
        if (pid == (GUI_PROG_COUNT - 1))
        {
            pane_exit();
            return SYSTEM_REBOOT;
        }
        // we have to be careful with SeeSH and tear down GUI
        else if (pid == (GUI_PROG_COUNT - 2))
        {
            pane_exit();
            // pass the buck
            if (gui_prog_lst[pid].main(1, NULL) == SYSTEM_REBOOT)
                return SYSTEM_REBOOT;
        }
        // programs that can be launched from the GUI will have to handle the
        // lack of command line args
        else
            gui_prog_lst[pid].main(1, NULL);
    }

    // leave graphics mode
    pane_exit();
    return EXIT_SUCCESS;
}
