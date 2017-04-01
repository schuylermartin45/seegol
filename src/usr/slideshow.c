/*
** File:    slideshow.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Slideshow program that displays images to the screen
*/

/** Headers    **/
#include "../kern/gcc16.h"
#include "slideshow.h"

#include "../kern/kio.h"
#include "../gl/gl_lib.h"
#include "../gl/img_fids.h"

// limitations on image scaling
#define MIN_SCALE   1
#define MAX_SCALE   9

/*
** Initializes program structure
**
** @param prog Program pointer to set
*/
void slideshow_init(Program* prog)
{
    prog->name = "Slideshow";
    prog->desc =
        "Displays images on the machine. 'q' to quit. Arrow keys to navigate. "
        "Any digit '1-9' will scale the image. Image and scale can be given "
        "as arguments.";
    prog->usage = "[f_id] [scale]";
    prog->main = &slideshow_main;
}

/*
** Main method for the program
*/
uint8_t slideshow_main(uint8_t argc, char* argv[])
{
    // usage check
    if (argc > 3)
        return ERR_PROG_USAGE;

    // default to VGA13
    gl_enter(VGA_MODE_13);

    // default file and scale; first file, no scaling
    uint16_t fid = 0;
    uint16_t scale = 1;

    // pick single image mode
    if (argc > 1)
    {
        fid = kio_str_int(argv[1], 10);
        // check if image exists in the table
        if (fid > GL_IMG_TBL_SIZE - 1)
        {
            gl_exit();
            return ERR_PROG_BAD_ARGS;
        }
        // check if scale was specified by the user
        if (argc > 2)
        {
            // limiting scale from 1-9
            scale = kio_str_int(argv[2], 10);
            if ((scale < MIN_SCALE) || (scale > MAX_SCALE))
            {
                gl_exit();
                return ERR_PROG_BAD_ARGS;
            }
        }
        // draw one image
        gl_draw_img_center_scale(fid, scale);
        // block for user input
        kio_wait_key('q');
    }
    // default operation mode: slideshow of all images, with some user 
    // interactive features
    else
    {
        // block for user input and commands
        uint16_t key = '\0';
        do
        {
            gl_draw_img_center_scale(fid, scale);
            key = kio_getchr_16bit();
            switch ((char)key)
            {
                // increase scale
                case '1': case '2': case '3': 
                case '4': case '5': case '6':
                case '7': case '8': case '9':
                    scale = key - '0';
                    break;
            }
            switch (key)
            {
                case KEY_ARROW_RT:
                    ++fid;
                    break;
                case KEY_ARROW_LT:
                    if (fid > 0)
                    {
                        --fid;
                        break;
                    }
            }
            // break if we've seen everything
            if (fid >= GL_IMG_TBL_SIZE)
                break;
            gl_clrscr();
        }
        while ((char)key != 'q');
    }

    // exit graphics mode
    gl_exit();

    return EXIT_SUCCESS;
}
