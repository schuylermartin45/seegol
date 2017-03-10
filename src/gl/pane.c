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
#define RGB_PANE_TITLE      RGB( 20,  26, 68)

// fraction used to determine
#define PANE_W_FRAC_PAD 36
#define PANE_H_FRAC_PAD 32

// default font scale
#define DEFAULT_FONT_SCALE  1
// max font scale to test when trying to auto-scale text to fit in a pane
#define MAX_FONT_SCALE      5
// same scale criteria but for images 
#define DEFAULT_IMG_SCALE   1
#define MAX_IMG_SCALE       5

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
** Initialize a graphical mode for pane-based programs
**
** @param mode Graphics mode to select
*/
void pane_enter(uint8_t mode)
{
    // start graphics mode, if needed
    if (gl_get_mode() != mode)
        gl_enter(mode);
    // calculate common values, now that we have some info on the display mode
    // size of the view window
    fr_w = gl_getw();
    fr_h = gl_geth();
    // padding on pane; also determines the upper-left point on the pane
    pane_pad.x = fr_w / PANE_W_FRAC_PAD;
    pane_pad.y = fr_h / PANE_H_FRAC_PAD;
    // pane width and height
    pane_wh.x = fr_w - (2 * pane_pad.x);
    pane_wh.y = fr_h - (2 * pane_pad.y);
    // text boundary; forces word wrap
    pane_w_bound = fr_w - pane_pad.y;
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
    gl_draw_rect_wh(pane_pad, pane_wh.x, pane_wh.y, RGB_OFF_WHITE);
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
** Draws a title for the top of a pane
**
** @param title Title string or NULL if no title is required
** @return Height of the title font
*/
static uint16_t __pane_draw_top_title(char* title)
{
    if (title == NULL)
        return 0;
    Point_2D ul = pane_pad;
    Point_2D bb;
    gl_draw_str_bb(ul, title, DEFAULT_FONT_SCALE, pane_w_bound, &bb);
    // draw a background rectangle around the title
    gl_draw_rect_wh(pane_pad, pane_wh.x, bb.y, RGB_PANE_TITLE);
    // draw the title to the screen
    gl_draw_str_scale(ul, RGB_OFF_WHITE, RGB_OFF_WHITE, title,
        DEFAULT_FONT_SCALE, pane_w_bound);
    return bb.y;
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
    uint8_t title_scale = DEFAULT_FONT_SCALE;
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
    gl_draw_str_bb(sub_ul, sub, DEFAULT_FONT_SCALE, pane_w_bound, &sub_bb);
    // center the subtitle
    sub_ul.x = (sub_ul.x + pane_wh.x - sub_bb.x) / 2;
    // put the subtitle under the title
    sub_ul.y = title_ul.y + title_bb.y + pane_pad.y;
    gl_draw_str_scale(sub_ul, RGB_HSC, RGB_HSC, sub, DEFAULT_FONT_SCALE,
        pane_w_bound);
}

/*
** Draws a pane with a title and text
**
** @param title Title string
** @param text Text string
*/
void pane_draw_title_text(char* title, char* text)
{
    // set-up pane and title
    __pane_draw_bg();
    uint16_t title_h = __pane_draw_top_title(title);

    // draw text under the title
    gl_draw_str_scale(PT2(pane_pad.x, title_h + pane_pad.y),
        RGB_HSC, RGB_HSC, text, DEFAULT_FONT_SCALE, pane_w_bound);
}

/*
** Draws a pane with text
**
** @param text Text string
*/
void pane_draw_text(char* text)
{
    pane_draw_title_text(NULL, text);
}

/*
** Draws a pane with a title and an image, centered
**
** @param title Title string
** @param fid File id of the image
*/
void pane_draw_title_img(char* title, uint8_t fid)
{
    // set-up pane and title
    __pane_draw_bg();
    __pane_draw_top_title(title);

    // retrieve image statistics
    Point_2D img_dims;
    gl_img_stat(fid, &img_dims);
    // auto-size and center the image on the screen
    uint8_t img_scale = DEFAULT_IMG_SCALE;
    // try to find the best-fitting scale that keeps the image on the pane
    for (uint8_t s=MAX_IMG_SCALE; s>=img_scale; --s)
    {
        if ((img_dims.x * s) <= (pane_wh.x - (3 * pane_pad.x)))
        {
            if ((img_dims.y * s) <= (pane_wh.y - (3 * pane_pad.y)))
            {
                img_scale = s;
                break;
            }
        }
    }
    gl_draw_img_center_scale(fid, img_scale);
}

/*
** Draws a pane with an image, centered
**
** @param fid File id of the image
*/
void pane_draw_img(uint8_t fid)
{
    pane_draw_title_img(NULL, fid);
}

/*
** Draws a pane with a title, an image to the right, and text to the left
**
** @param title Title string
** @param fid File id of the image
** @param text Text to go with the image
*/
void pane_draw_title_img_text(char* title, uint8_t fid, char* text)
{
    // set-up pane and title
    __pane_draw_bg();
    uint16_t title_h = __pane_draw_top_title(title);

    // set an image to the right of the text (fitting within the pane),
    // centered vertically in the pane
    Point_2D img_dims;
    gl_img_stat(fid, &img_dims);
    Point_2D img_ul;
    // image is put on the pane to have left padding from the edge of the pane
    img_ul.x = fr_w - (2 * pane_pad.x) - img_dims.x;
    // vertically, the image is centered in the pane
    img_ul.y = pane_pad.y + (pane_wh.y / 2) - (img_dims.y / 2);
    gl_draw_img_scale(fid, img_ul, DEFAULT_IMG_SCALE);

    // set the text to the left, right-bounded by the left of the image, with
    // some padding
    gl_draw_str_scale(PT2(pane_pad.x, title_h + pane_pad.y),
        RGB_HSC, RGB_HSC, text, DEFAULT_FONT_SCALE, img_ul.x - pane_pad.x);
}

/*
** Draws a pane with an image to the right and text to the left
**
** @param fid File id of the image
** @param text Text to go with the image
*/
void pane_draw_img_text(uint8_t fid, char* text)
{
    pane_draw_title_img_text(NULL, fid, text);
}


