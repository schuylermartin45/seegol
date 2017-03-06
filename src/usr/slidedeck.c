/*
** File:    slidedeck.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Slidedeck program that displays panes, like a poor man's power
**              point; a presentation
*/

/** Headers    **/
#include "../kern/gcc16.h"
#include "slidedeck.h"

#include "../kern/kio.h"
#include "../gl/img_fids.h"
#include "../gl/pane.h"

/** Macros    **/
#define SLIDES_SIZE 1

/*
** Initializes program structure
**
** @param prog Program pointer to set
*/
void slidedeck_init(Program* prog)
{
    prog->name = "slidedeck";
    prog->desc =
        "Slidedeck Program. Displays a presentation. 'q' to quit. 'space|n|.' "
        "to\n"
        "  advance to the next slide, 'p|,' to go back a slide.";
    prog->usage = "";
    prog->main = &slidedeck_main;
}

/*
** Main method for the program
*/
uint8_t slidedeck_main(uint8_t argc, char* argv[])
{
    // default to VGA13
    pane_enter(VGA_MODE_13);

    // slidedeck of all images
    // block for user input and commands
    char key = '\0';
    uint8_t slide_id = 0;
    do
    {
        pane_draw_title("HSC Pane Demo", "Schuyler Martin");
        key = kio_getchr();
        switch (key)
        {
            // slides can go forward and backwards
            case 'n':
            case '.':
            case KEY_SPACE:
                ++slide_id;
                break;
            case 'p':
            case ',':
                if (slide_id > 0)
                {
                    --slide_id;
                    break;
                }
        }
        // break if we've seen everything
        if (slide_id >= SLIDES_SIZE)
            break;
    }
    while (key != 'q');

    // exit graphics mode
    pane_exit();

    return EXIT_SUCCESS;
}
