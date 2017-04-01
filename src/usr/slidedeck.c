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
#define SLIDES_SIZE 3

/*
** Initializes program structure
**
** @param prog Program pointer to set
*/
void slidedeck_init(Program* prog)
{
    prog->name = "Slidedeck";
    prog->desc =
        "A presentation about SeeGOL. 'q' to quit. Arrow keys to navigate.";
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
    uint16_t key = '\0';
    uint8_t slide_id = 0;
    do
    {
        switch(slide_id)
        {
            case 0:
                pane_draw_title("HSC SeeGOL",
                    "Shoyler's Extremely Experimental "
                    "Graphical Operating Library"
                );
                break;
            case 1:
                pane_draw_title_img_text("About", IMG_FID_HSC,
                    "SeeGOL was developed by Schuyler Martin as his capstone "
                    "project. He has been studying CS at RIT as a BS/MS "
                    "student. During that time, he focued his work in systems "
                    "programming, CG, and CV. SeeGOL is evidence of the "
                    "skills he has learned in that time.\n"
                    "        - Shoyler, 2017"
                );
                break;
            case 2:
                pane_draw_img(IMG_FID_HSC);
                break;
        }
        key = kio_getchr_16bit();
        switch (key)
        {
            // slides can go forward and backwards
            case KEY_ARROW_RT:
                ++slide_id;
                break;
            case KEY_ARROW_LT:
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
    while ((char)key != 'q');

    // exit graphics mode
    pane_exit();

    return EXIT_SUCCESS;
}
