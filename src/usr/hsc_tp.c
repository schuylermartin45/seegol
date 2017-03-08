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

#include "../kern/kio.h"
#include "../gl/gl_lib.h"
#include "../gl/img_fids.h"
#include "../gl/pane.h"

// info about the Macbeth color chart
#define MACBETH_ROWS 4
#define MACBETH_COLS 6
#define HSC_NAME     "HENRIETTA SEMICONDUCTOR CORPORATION"
// pane drawing tests
#define PANE_TEST_TITLE "Wish You Were Here - Pink Floyd"
#define PANE_TEST_TEXT  \
    "So, so you think you can tell. "\
    "Heaven from hell. "\
    "Blue skies from pain. "\
    "Can you tell a green field. "\
    "From a cold steel rail? "\
    "A smile from a veil? "\
    "Do you think you can tell? "\
    "Did they get you to trade. "\
    "Your heroes for ghosts? "\
    "Hot ashes for trees? "\
    "Hot air for a cool breeze? "\
    "Cold comfort for change? "\
    "And did you exchange. "\
    "A walk on part in the war. "\
    "For a lead role in a cage? "\
    "How I wish, how I wish you were here. "\
    "We're just two lost souls. "\
    "Swimming in a fish bowl. "\
    "Year after year. "\
    "Running over the same old ground. "\
    "And how we found..." //"The same old fears. "\"Wish you were here. "

/*
** Initializes program structure
**
** @param prog Program pointer to set
*/
void hsc_tp_init(Program* prog)
{
    prog->name = "hsc_tp";
    prog->desc =
        "HSC Graphical Test Pattern Program. 'q' to quit. 'mode' picks a "
        "VGA driver\n  mode. '-i' tests the image draw function. '-m' proced"
        "urally draws the\n  Macbeth chart. '-t' tests string drawing. '-o' "
        "draw the original test image.\n  '-p' tests pane drawing.";
    prog->usage = "mode [-i|-m|-t|-o|-p]";
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
    // round off may occur in some dimensions, leaving a some unused space
    uint16_t w = gl_getw() / MACBETH_COLS;
    uint16_t h = gl_geth() / MACBETH_ROWS;
    // center the pattern
    uint16_t x_offset = (gl_getw() % MACBETH_COLS) / 2;
    uint16_t y_offset = (gl_geth() % MACBETH_ROWS) / 2;
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
            gl_draw_rect_wh(PT2((w*col) + x_offset, (h*row) + y_offset),
                w, h, *color++);
        }
    }
}

/*
** Draw HSC logo, reminiscent of the PCjr boot-screen
*/
static void __hsc_tp_draw_HSC(void)
{
    // white border
    gl_draw_rect_wh(PT2(50, 30), 220, 140, RGB_WHITE);
    // H
    gl_draw_rect_wh(PT2( 65, 40), 20, 120, RGB_HSC);
    gl_draw_rect_wh(PT2( 85, 95), 20,  10, RGB_HSC);
    gl_draw_rect_wh(PT2(105, 40), 20, 120, RGB_HSC);
    // S
    gl_draw_rect_wh(PT2(135,  43), 20, 60, RGB_HSC);
    gl_draw_rect_wh(PT2(155,  40), 40, 25, RGB_HSC);
    gl_draw_rect_wh(PT2(155,  90), 20, 20, RGB_HSC);
    gl_draw_rect_wh(PT2(175,  95), 20, 63, RGB_HSC);
    gl_draw_rect_wh(PT2(135, 135), 40, 25, RGB_HSC);
    // C
    gl_draw_rect_wh(PT2(205,  43), 20, 114, RGB_HSC);
    gl_draw_rect_wh(PT2(225,  40), 40,  20, RGB_HSC);
    gl_draw_rect_wh(PT2(225, 140), 40,  20, RGB_HSC);
    // I'm lazy at this point...screw the z axis.
    // white stripes on the logo
    gl_draw_rect_wh(PT2(50, 135), 220, 3, RGB_WHITE);
    gl_draw_rect_wh(PT2(50, 143), 220, 3, RGB_WHITE);
    gl_draw_rect_wh(PT2(50, 150), 220, 3, RGB_WHITE);
    // draw a string
    gl_draw_str(PT2(20, 180), RGB(255, 100, 0), RGB_HSC, HSC_NAME);
}

/*
** Draw test function for string printing
*/
static void __hsc_tp_draw_str(void)
{
    // TEST 0: basic character draw
    char* str0 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789 .";
    gl_draw_str(PT2(0, 0), RGB_WHITE, RGB_HSC, str0);
    // TEST 1: new line
    char* str1 = "NEWLINE TEST\nNEWLINE TEST\nNEWLINE TEST";
    gl_draw_str(PT2(20, 20), RGB_HSC, RGB_WHITE, str1);
    // TEST 2: word wrap
    char* str2 = "WORD WRAP WORD WORD WRAP WORD WRAP WORD WRAP WORD WRAP "
                 "WORD WRAP WORD WORD WRAP WORD WRAP WORD WRAP WORD WRAP ";
    gl_draw_str(PT2(0, 100), RGB_WHITE, RGB_BLACK, str2);
    // TEST 3: sprintf and transparent backgrounds
    char* str3 = "BIN %b  HEX %x WORDS WORDS WORDS NEWLINE\nWORDS WORDS";
    uint16_t num = 42;
    gl_draw_strf(PT2(50, 160), RGB_WHITE, RGB_WHITE, str3, &num, &num);
    char* str4 = "|%6b|%6x| left pad of 6 both.";
    gl_draw_strf(PT2(0, 140), RGB_BLACK, RGB_WHITE, str4, &num, &num);
}

/*
** Main method for HSC Test Pattern program
*/
uint8_t hsc_tp_main(uint8_t argc, char* argv[])
{
    // usage check
    if ((argc < 2) && (argc > 3))
        return ERR_PROG_USAGE;
    // enter graphics mode, based on user input
    uint16_t vga_mode = 0;
    if (kio_strcmp(argv[1], "13"))
    {
        vga_mode = VGA_MODE_13;
    }
    else
        return ERR_PROG_BAD_ARGS;
    gl_enter(vga_mode);

    // draw string test
    if ((argc == 3) && (kio_strcmp(argv[2], "-t")))
        __hsc_tp_draw_str();
    // draw image test
    else if ((argc == 3) && (kio_strcmp(argv[2], "-i")))
        // HSC logo from Kailey's original PNG
        gl_draw_img_center_scale(IMG_FID_HSC, 3);
    // draw colors test (Macbeth color chart)
    else if ((argc == 3) && (kio_strcmp(argv[2], "-m")))
        __hsc_tp_draw_board();
    // original HSC Test Pattern, logo made in rectangles
    else if ((argc == 3) && (kio_strcmp(argv[2], "-o")))
    {
        __hsc_tp_draw_board();
        __hsc_tp_draw_HSC();
    }
    // tests the pane drawing library; this test is "self contained" because
    // all the other tests rely on the GL package and the Pane package has its
    // own intialization system.
    else if ((argc == 3) && (kio_strcmp(argv[2], "-p")))
    {
        pane_enter(vga_mode);
        pane_draw_title_text(PANE_TEST_TITLE, PANE_TEST_TEXT);
    }
    // putting it all together: transparent HSC Logo w/ color chart and text
    else if (argc < 3)
    {
        __hsc_tp_draw_board();
        // white background to make the logo pop out
        gl_draw_rect_wh(PT2(70, 65), 180, 70, RGB_WHITE);
        gl_draw_img_center_scale(IMG_FID_HSC, 2);
        // draw a string; hides crappy compressed text in the image
        gl_draw_str(PT2(20, 145), RGB(255, 100, 0), RGB_HSC, HSC_NAME);
    }
    // error; exit graphics mode
    else
    {
        gl_exit();
        return ERR_PROG_BAD_ARGS;
    }

    // block for user input
    kio_wait_key('q');

    // exit graphics mode
    gl_exit();

    return EXIT_SUCCESS;
}
