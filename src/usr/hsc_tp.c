/*
** File:    hsc_tp.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Graphical test program that displays the HSC logo and a test
**              pattern
*/

/** Headers    **/
#include "../kern/gcc16.h"
#include "hsc_tp.h"

/*
** Initializes program structure
**
** @param prog Program pointer to set
*/
void hsc_tp_init(Program* prog)
{
    prog->name = "hsc_tp";
    prog->desc = "HSC Graphical Test Pattern Program. 'q' to quit.";
    prog->usage = "mode";
    prog->main = &hsc_tp_main;
}

/*
** Draw checkboard pattern across the entire screen
*/
static void __hsc_tp_draw_board(void)
{
    // calculate the size of each piece based on the screen dimensions
    uint16_t w = gl_getw() / 8;
    uint16_t h = gl_geth() / 8;
    uint16_t color = 0;

    for (uint8_t row=0; row<8; ++row)
    {
        for (uint8_t col=0; col<8; ++col)
        {
            gl_draw_rect_wh(w*col, h*row, w, h, color);
            // next color
            color++;
        }
    }
}

/*
** Draw HSC logo, reminiscent of the PCjr boot-screen
*/
static void __hsc_tp_draw_HSC(void)
{
    // white border
    gl_draw_rect_wh(50, 30, 220, 140, 63);
    // H
    gl_draw_rect_wh( 65, 40, 20, 120, 8);
    gl_draw_rect_wh( 85, 95, 20,  10, 8);
    gl_draw_rect_wh(105, 40, 20, 120, 8);
    // S
    gl_draw_rect_wh(135,  43, 20, 60, 8);
    gl_draw_rect_wh(155,  40, 40, 25, 8);
    gl_draw_rect_wh(155,  90, 20, 20, 8);
    gl_draw_rect_wh(175,  95, 20, 63, 8);
    gl_draw_rect_wh(135, 135, 40, 25, 8);
    // C
    gl_draw_rect_wh(205,  43, 20, 114, 8);
    gl_draw_rect_wh(225,  40, 40,  20, 8);
    gl_draw_rect_wh(225, 140, 40,  20, 8);
    // I'm lazy at this point...screw the z axis.
    // white stripes on the logo
    gl_draw_rect_wh(50, 135, 220, 3, 63);
    gl_draw_rect_wh(50, 143, 220, 3, 63);
    gl_draw_rect_wh(50, 150, 220, 3, 63);
}

/*
** Main method for HSC Test Pattern program
*/
uint8_t hsc_tp_main(uint8_t argc, char* argv[])
{
    // usage check
    if (argc != 2)
        return ERR_PROG_USAGE;
    // enter graphics mode, based on user input
    gl_init();
    if (kio_strcmp(argv[1], "13"))
        gl_enter(VGA_MODE_13);
    else
        return ERR_PROG_BAD_ARGS;

    // background
    __hsc_tp_draw_board();
    // foreground
    __hsc_tp_draw_HSC();

    // block for user input
    kio_wait_key('q');

    // exit graphics mode
    gl_exit();

    return EXIT_SUCCESS;
}
