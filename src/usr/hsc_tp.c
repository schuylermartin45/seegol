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

// info about the Macbeth color chart
#define MACBETH_ROWS 4
#define MACBETH_COLS 6

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
** Draw checkboard pattern across the entire screen, based on the Macbeth
** Color Chart, used in photography and computer vision.
**   Example values from:
**     http://www.rags-int-inc.com/PhotoTechStuff/AcrCalibration/
**       MacbethCC-NumsRGB.jpg
*/
static void __hsc_tp_draw_board(void)
{
    // calculate the size of each piece based on the screen dimensions
    uint16_t w = gl_getw() / MACBETH_COLS;
    uint16_t h = gl_geth() / MACBETH_ROWS;
    // set up the chart
    RGB_8 colors[MACBETH_ROWS * MACBETH_COLS] =
    {
        // row 1
        { 94,  28,  13}, {241, 148, 108},
        { 97, 119, 171}, { 90, 103,  39},
        {164, 131, 196}, {140, 253, 153},
        // row 2
        {255, 116,  21}, {  7,  47, 122},
        {222,  29,  42}, { 69,   0,  68},
        {187, 255,  19}, {255, 142,   0},
        // row 3
        {  0,   0, 142}, { 64, 173,  38},
        {203,   0,   0}, {255, 217,   0},
        {207,   3, 124}, {  0, 148, 189},
        // row 4
        {255, 255, 255}, {249, 249, 249},
        {180, 180, 180}, {117, 117, 117},
        { 53,  53,  53}, {  0,   0,   0},
    };
    // iterator
    RGB_8* color = colors;

    // iterate over all the colors
    for (uint8_t row=0; row<MACBETH_ROWS; ++row)
    {
        for (uint8_t col=0; col<MACBETH_COLS; ++col)
        {
            gl_draw_rect_wh(w*col, h*row, w, h, color++);
        }
    }
}

/*
** Draw HSC logo, reminiscent of the PCjr boot-screen
*/
static void __hsc_tp_draw_HSC(void)
{
    // white border
    gl_draw_rect_wh(50, 30, 220, 140, &RGB_8_WHITE);
    // H
    gl_draw_rect_wh( 65, 40, 20, 120, &RGB_8_HSC);
    gl_draw_rect_wh( 85, 95, 20,  10, &RGB_8_HSC);
    gl_draw_rect_wh(105, 40, 20, 120, &RGB_8_HSC);
    // S
    gl_draw_rect_wh(135,  43, 20, 60, &RGB_8_HSC);
    gl_draw_rect_wh(155,  40, 40, 25, &RGB_8_HSC);
    gl_draw_rect_wh(155,  90, 20, 20, &RGB_8_HSC);
    gl_draw_rect_wh(175,  95, 20, 63, &RGB_8_HSC);
    gl_draw_rect_wh(135, 135, 40, 25, &RGB_8_HSC);
    // C
    gl_draw_rect_wh(205,  43, 20, 114, &RGB_8_HSC);
    gl_draw_rect_wh(225,  40, 40,  20, &RGB_8_HSC);
    gl_draw_rect_wh(225, 140, 40,  20, &RGB_8_HSC);
    // I'm lazy at this point...screw the z axis.
    // white stripes on the logo
    gl_draw_rect_wh(50, 135, 220, 3, &RGB_8_WHITE);
    gl_draw_rect_wh(50, 143, 220, 3, &RGB_8_WHITE);
    gl_draw_rect_wh(50, 150, 220, 3, &RGB_8_WHITE);
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
