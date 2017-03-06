/*
** File:    pane.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Library that provides a common frame in order to put display
**              information on the OS. Some people call these windows. I call
**              them pane.s
*/

/** Headers    **/
#include "../kern/gcc16.h"
#include "pane.h"

#include "gl_lib.h"

// Common colors used in panes
#define RGB_OFF_WHITE       RGB(230, 230, 230)
#define RGB_DROP_SHADOW     RGB( 30,  30,  30)

// fraction used to determine
#define PANE_W_FRAC_PAD 36
#define PANE_H_FRAC_PAD 32

// screen dimensions
static uint16_t fr_w;
static uint16_t fr_h;
// w and h padding information for the pane
static Point_2D pane_pad;

/*
** Enter a graphical mode for pane-based programs
**
** @param mode Graphics mode to select
*/
void pane_enter(uint8_t mode)
{
    // start graphics mode
    gl_init();
    gl_enter(mode);
    // calculate common values, now that we have some info on the display mode
    // size of the view window
    fr_w = gl_getw();
    fr_h = gl_geth();
    // padding on pane
    pane_pad.x = fr_w / PANE_W_FRAC_PAD;
    pane_pad.y = fr_h / PANE_H_FRAC_PAD;
}

/*
** Exit graphical pane mode and go back to text
*/
void pane_exit(void)
{
    gl_exit();
}

/*
** Draws background of a pane; standard across all panes
*/
static void __pane_draw_bg()
{
    gl_clrscr();
    // clear screen with background color
    gl_draw_rect_wh(PT2(0, 0), fr_w, fr_h, RGB_HSC);
    // draw the pane on top of the background color
    gl_draw_rect_wh(pane_pad, fr_w - (2 * pane_pad.x), fr_h - (2 * pane_pad.y),
        RGB_OFF_WHITE);
    // bump-map the screen borders because we want to look cool
    // bottom drop shadow
    gl_draw_rect_wh(
        PT2(2 * pane_pad.x, fr_h - pane_pad.y),
        fr_w - (3 * pane_pad.x),
        pane_pad.y / 2,
        RGB_DROP_SHADOW
    );
    // right-hand drop shadow
    gl_draw_rect_wh(
        PT2(fr_w - pane_pad.x, 2 * pane_pad.y),
        pane_pad.x / 2,
        fr_h - (3 * pane_pad.y) + (pane_pad.y / 2),
        RGB_DROP_SHADOW
    );
}

/*
** Draws a title pane
**
** @param title Title string
** @param subtitle Subtitle string
*/
void pane_draw_title(char* title, char* subtitle)
{
    __pane_draw_bg();
    // center the title text and scale up
    gl_draw_str_scale(PT2(pane_pad.x, 50), RGB_HSC, RGB_HSC, title, 3);
    // subtitle goes underneath that
    gl_draw_str(PT2(100,100), RGB_HSC, RGB_HSC, subtitle);
}
