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

#include "../kern/kio.h"
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

// strf pattern used for options in a prompt menu
#define OPT_PATTERN         "%D. - %s"

// screen dimensions
static uint16_t fr_w;
static uint16_t fr_h;
// w and height of the pane
static Point_2D pane_wh;
// w and h padding information for the pane
static Point_2D pane_pad;
// width boundary of the pane; keeps the text from going out of the window
static uint16_t pane_w_bound;

// theme color settings, set by user program or defaults to the HSC theme
RGB_8 thm_b_pane;
RGB_8 thm_f_pane;
RGB_8 thm_b_title;
RGB_8 thm_f_title;
RGB_8 thm_text;
RGB_8 thm_b_select;
RGB_8 thm_f_select;
RGB_8 thm_drop_shadow;

/*
** Draws background of a pane; standard across all panes
*/
static void __pane_draw_bg()
{
    gl_clrscr();
    // clear screen with background color
    gl_draw_rect_wh(PT2(0, 0), fr_w, fr_h, thm_b_pane);
    // draw the pane on top of the background color
    gl_draw_rect_wh(pane_pad, pane_wh.x, pane_wh.y, thm_f_pane);
    // bump-map the screen borders because we want to look cool
    // bottom drop shadow
    gl_draw_rect_wh(
        PT2(2 * pane_pad.x, fr_h - pane_pad.y),
        fr_w - (3 * pane_pad.x),
        pane_pad.y / 2,
        thm_drop_shadow
    );
    // right-hand drop shadow
    gl_draw_rect_wh(
        PT2(fr_w - pane_pad.x, 2 * pane_pad.y),
        pane_pad.x / 2,
        fr_h - (3 * pane_pad.y) + (pane_pad.y / 2),
        thm_drop_shadow
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
    gl_draw_rect_wh(pane_pad, pane_wh.x, bb.y, thm_b_title);
    // draw the title to the screen
    gl_draw_str_scale(ul, thm_f_title, thm_f_title, title,
        DEFAULT_FONT_SCALE, pane_w_bound);
    return bb.y;
}

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
    // use theme defaults
    pane_set_theme(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

/*
** Sets the theme for panes. These settings are persistent. If NULL is provided
** for any argument, that value is set to the default color value
**
** @param b_pane Pane background color
** @param f_pane Pane foreground color
** @param b_title Background color of the title bar
** @param f_title Foreground (text) color of the title bar
** @param b_select Background text selection color
** @param f_select Foreground (text) text selection color
** @param text Text color
** @param drop_shadow Boolean controls whether the drop shadow is visible
*/
void pane_set_theme(
    RGB_8* b_pane,   RGB_8* f_pane,
    RGB_8* b_title,  RGB_8* f_title,
    RGB_8* b_select, RGB_8* f_select,
    RGB_8* text,     RGB_8* drop_shadow)
{
    thm_b_pane      = (b_pane == NULL)      ? RGB_HSC           : *b_pane;
    thm_f_pane      = (f_pane == NULL)      ? RGB_OFF_WHITE     : *f_pane;
    thm_b_title     = (b_title == NULL)     ? RGB_PANE_TITLE    : *b_title;
    thm_f_title     = (f_title == NULL)     ? RGB_OFF_WHITE     : *f_title;
    thm_b_select    = (b_select == NULL)    ? RGB_HSC           : *b_select;
    thm_f_select    = (f_select == NULL)    ? RGB_OFF_WHITE     : *f_select;
    thm_text        = (text == NULL)        ? RGB_HSC           : *text;
    thm_drop_shadow = (drop_shadow == NULL) ? RGB_DROP_SHADOW   : *drop_shadow;
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
    gl_draw_str_scale(title_ul, thm_text, thm_text, title, title_scale,
        pane_w_bound);

    // subtitle goes underneath the title, centered
    Point_2D sub_ul = pane_pad;
    Point_2D sub_bb;
    gl_draw_str_bb(sub_ul, sub, DEFAULT_FONT_SCALE, pane_w_bound, &sub_bb);
    // center the subtitle
    sub_ul.x = (sub_ul.x + pane_wh.x - sub_bb.x) / 2;
    // put the subtitle under the title
    sub_ul.y = title_ul.y + title_bb.y + pane_pad.y;
    gl_draw_str_scale(sub_ul, thm_text, thm_text, sub, DEFAULT_FONT_SCALE,
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
        thm_text, thm_text, text, DEFAULT_FONT_SCALE, pane_w_bound);
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
        thm_text, thm_text, text, DEFAULT_FONT_SCALE, img_ul.x - pane_pad.x);
}

/*
** Draws a prompt pane, which allows the user to pick an option from a menu
**
** @param prompt Prompt string
** @param optc Option count, number of options in the menu
** @param optv Option values, array of strings holding user options
** @return 0-indexed ID of the option selected by the user
*/
uint8_t pane_draw_prompt(char* prompt, uint8_t optc, char* optv[])
{
    __pane_draw_bg();
    uint16_t prompt_h = __pane_draw_top_title(prompt);

    // control for user input, redrawing on each change
    uint8_t opt = 0;
    // tracking the previous option will reduce drawing calls on invalid input
    uint8_t old_opt = 1;
    uint16_t ch;
    do
    {
        // redraw
        if (opt != old_opt)
        {
            Point_2D opt_ul = {pane_pad.x, prompt_h + pane_pad.y};
            for(uint8_t i=0; i<optc; ++i)
            {
                Point_2D bb;
                // calculate the size of the string to be printed
                gl_draw_strf_bb(opt_ul, OPT_PATTERN, DEFAULT_FONT_SCALE,
                    pane_w_bound, &bb, &i, optv[i]);
                // selectively draw the selected text
                if (opt == i)
                {
                    gl_draw_strf_scale(opt_ul, thm_b_select, thm_f_select,
                        OPT_PATTERN, DEFAULT_FONT_SCALE, pane_w_bound, &i,
                        optv[i]
                    );
                }
                else
                {
                    gl_draw_strf_scale(opt_ul, thm_f_pane, thm_text,
                        OPT_PATTERN, DEFAULT_FONT_SCALE, pane_w_bound, &i,
                        optv[i]
                    );
                }
                // advance the cursor
                opt_ul.y += pane_pad.y + bb.y;
            }
        }
        // record the old option for redrawing purposes
        old_opt = opt;
        // prompt for input
        ch = kio_getchr_16bit();
        // parse the input as a 16bit input code
        if ((char)ch == '\0')
        {
            // arrow key processing
            switch (ch)
            {
                case KEY_ARROW_UP:
                    if (opt == 0)
                        opt = optc;
                    --opt;
                    break;
                case KEY_ARROW_DN:
                    ++opt;
                    break;
            }
            // keep the option within the bounds
            opt %= optc;
        }
        // pick an option using number IDs
        else if (((char)ch >= '0') && ((char)ch < ('0' + (char)optc)))
            opt = (char)ch - '0';
    } while(((char)ch != '\r') && ((char)ch != '\n'));

    // prompts clean-up after themselves
    gl_clrscr();
    return opt;
}
