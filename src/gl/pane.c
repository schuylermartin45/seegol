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
#include "see_font.h"

// Common colors used in panes
#define RGB_OFF_WHITE       RGB(230, 230, 230)
#define RGB_DROP_SHADOW     RGB( 30,  30,  30)

// fraction used to determine
#define PANE_W_FRAC_PAD 36
#define PANE_H_FRAC_PAD 32

// max font scale to test when trying to auto-scale text to fit in a pane
#define MAX_FONT_SCALE  5

// screen dimensions
static uint16_t fr_w;
static uint16_t fr_h;
// w and height of the pane
static Point_2D pane_wh;
// w and h padding information for the pane
static Point_2D pane_pad;
// width boundary of the pane; keeps the text from going out of the window
static uint16_t pane_w_bound;

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
    // pane width and height
    pane_wh.x = fr_w - (2 * pane_pad.x);
    pane_wh.y = fr_h - (2 * pane_pad.y);
    // text boundary; forces word wrap
    pane_w_bound = fr_w - pane_pad.y;
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
** @param sub Subtitle string
*/
void pane_draw_title(char* title, char* sub)
{
    __pane_draw_bg();
    // center the title text and automatically pick the largest font size that
    // will fit in that region without word wrapping, if possible
    uint8_t title_scale = 1;
    Point_2D title_ul = pane_pad;
    Point_2D title_bb;
    // first try to find the best-fitting scale, horizontally
    for (uint8_t s=MAX_FONT_SCALE; s>=title_scale; --s)
    {
        gl_draw_str_bb(title_ul, title, s, pane_w_bound, &title_bb);
        // make sure that the text will fit and is of a reasonable height
        if ((title_bb.x < pane_wh.x) && (title_bb.y <= (pane_wh.y / 4)))
        {
            title_scale = s;
            break;
        }
    }
    // then "center" the title on the pane, in both directions
    title_ul.x = (title_ul.x + pane_wh.x - title_bb.x) / 2;
    title_ul.y = (title_ul.y + pane_wh.y - title_bb.y) / 2;
    // actually draw the title to the screen
    gl_draw_str_scale(title_ul, RGB_HSC, RGB_HSC, title, title_scale,
        pane_w_bound);

    // subtitle goes underneath the title, centered
    Point_2D sub_ul = pane_pad;
    Point_2D sub_bb;
    gl_draw_str_bb(sub_ul, sub, 1, pane_w_bound, &sub_bb);
    // center the subtitle
    sub_ul.x = (sub_ul.x + pane_wh.x - sub_bb.x) / 2;
    // put the subtitle under the title
    sub_ul.y = title_ul.y + title_bb.y + pane_pad.y;
    gl_draw_str_scale(sub_ul, RGB_HSC, RGB_HSC, sub, 1, pane_w_bound);
}
