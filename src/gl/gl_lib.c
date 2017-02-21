/*
** File:   gl_lib.c
**
** Author: Schuyler Martin <sam8050@rit.edu>
**
** Description: User-level graphics library. The default is "text mode" and
**              this library handles some of the switching code that KIO
**              doesn't need to know about (i.e. using BIOS interrupts to
**              switch modes). Otherwise, GL more or less wraps around the VGA
**              driver function calls.
*/

/** Headers    **/
#include "../kern/gcc16.h"
#include "gl_lib.h"

// structure that manages the driver mode currently activated
// assumed to be text mode if graphics haven't been initialized yet
static VGA_Driver vga_driver;

/*
** Initialize the graphics library with default values. User should call this
** once per program
*/
void gl_init(void)
{
    // configure TEXT mode
    vga_driver.screen_w = TEXT_WIDTH;
    vga_driver.screen_h = TEXT_HEIGHT;
    vga_driver.frame_buff = (uint16_t*)TEXT_MEM_BEGIN;
    vga_driver.vga_mode = VGA_MODE_TEXT;
    // set all the functions here; shouldn't be called in text mode so use
    // NULL for now
    vga_driver.vga_enter = NULL;
    vga_driver.vga_clrscr = NULL;
    vga_driver.vga_put_pixel = NULL;
    vga_driver.vga_get_pixel = NULL;
    vga_driver.vga_draw_rect = NULL;
    vga_driver.vga_draw_rect_wh = NULL;
}

/*
** Enter a graphical mode
**
** @param mode Graphics mode to select
*/
void gl_enter(uint8_t mode)
{
    // start handling text debugging in the back up buffer
    kio_swap_fb();
    switch (mode)
    {
        case VGA_MODE_13:
            _vga13_enter(&vga_driver);
            break;
    }
}

/*
** Exit graphical mode and go back to text
*/
void gl_exit(void)
{
    // reset to the default mode
    gl_init();
    // punch it Chewie, make the jump back to text mode
    // (BIOS wipes graphics memory for free)
    __asm__ __volatile__("movb $0x03, %al\n");
    __asm__ __volatile__("movb $0x00, %ah\n");
    __asm__ __volatile__("int  $0x10\n");
    // hide the cursor
    __asm__ __volatile__("movb $0x00, %al\n");
    __asm__ __volatile__("movb $0x01, %ah\n");
    __asm__ __volatile__("movw $0x2607, %cx\n");
    __asm__ __volatile__("int  $0x10\n");
    // bring the text back to the primary display buffer
    kio_swap_fb();
}

/*
** Get the current width of the screen
**
** @param Width of the screen in the current mode
*/
uint16_t gl_getw(void)
{
    return vga_driver.screen_w;
}

/*
** Get the current height of the screen
**
** @param Height of the screen in the current mode
*/
uint16_t gl_geth(void)
{
    return vga_driver.screen_h;
}

/***** "Fast" Draw Functions (implemented in driver) *****/

/*
** Clears the video buffer
*/
void gl_clrscr(void)
{
    vga_driver.vga_clrscr();
}

/* Draw functions */

/*
** Draws a pixel to the screen. Coordinates start in the upper-left corner
**
** @param pt Pixel position
** @param color Pixel color to write. This is an index into the color palette
*/
void gl_put_pixel(Point_2D pt, RGB_8 color)
{
    vga_driver.vga_put_pixel(pt.x, pt.y, color);
}

/*
** Gets a pixel's color value. Coordinates start in the upper-left corner
**
** @param pt Pixel position
** @param color Pixel color written to the pixel position
*/
void gl_get_pixel(Point_2D pt, RGB_8* color)
{
    vga_driver.vga_get_pixel(pt.x, pt.y, color);
}

/*
** Draws a simple rectangle
**
** @param ur Upper-right coordinate on the screen
** @param ll Lower-left coordinate on the screen
** @param color Pixel color to write. This is an index into the color palette
*/
void gl_draw_rect(Point_2D ur, Point_2D ll, RGB_8 color)
{
    vga_driver.vga_draw_rect(ur.x, ur.y, ll.x, ll.y, color);
}

/*
** Draws a simple rectangle, using alternative parameter listings
**
** @param ul Upper-left coordinate on the screen
** @param w Width of the rectangle
** @param h Height of the rectangle
** @param color Pixel color to write. This is an index into the color palette
*/
void gl_draw_rect_wh(Point_2D ul, uint16_t w, uint16_t h, RGB_8 color)
{
    vga_driver.vga_draw_rect_wh(ul.x, ul.y, w, h, color);
}

/***** Generic Draw Functions (implemented in GL *****/

/*
** Draws a string, based on a custom-made bitmap font.
** "Transparent" backgrounds are achieved by setting the background and
** foreground colors to the same value.
**
** @param start Starting point to draw the string (upper left pixel)
** @param b_color Background color of the text
** @param f_color Foreground color of the text
** @param str String to draw
*/
void gl_draw_str(Point_2D start, RGB_8 b_color, RGB_8 f_color, char* str)
{
    // current position to draw
    Point_2D cur = start;
    // keeps character row, col counters as we move along
    Point_2D rc_cntr = {0, 0};
    // pad vertically
    cur.y += SEE_FONT_PAD_VERT;
    // loop until null byte
    while(*str != '\0')
    {
        // encounter a newline, jump ahead to the next line
        if (*str == '\n')
        {
            // jump a row of chars down
            rc_cntr.x = 0;
            ++rc_cntr.y;
            ++str;
        }
        else
        {
            uint8_t ch = *str;
            // enforce a newline if we are about to go out of bounds, in x
            cur.x = start.x
                + (rc_cntr.x * (SEE_FONT_WIDTH + (2 * SEE_FONT_PAD_HORZ)))
                + SEE_FONT_PAD_HORZ;
            if ((cur.x + SEE_FONT_WIDTH + (3 * SEE_FONT_PAD_HORZ)) 
                >= vga_driver.screen_w)
            {
                // reset before draw
                rc_cntr.x = 0;
                ++rc_cntr.y;
                cur.x = start.x + SEE_FONT_PAD_HORZ;
            }
            cur.y = start.y
                + (rc_cntr.y * (SEE_FONT_HEIGHT + (2 * SEE_FONT_PAD_VERT)))
                + SEE_FONT_PAD_VERT;
            // check for transparent backgrounds; both colors are the same
            if (!vga_RGB_8_cmp(b_color, f_color))
            {
                // quickly draw the background color using the rectangle draw
                // function and use this assumption for other optimizations
                vga_driver.vga_draw_rect_wh(
                    start.x + 
                        (rc_cntr.x * (SEE_FONT_WIDTH + (2
                            * SEE_FONT_PAD_HORZ))
                        ),
                    start.y +
                        (rc_cntr.y * (SEE_FONT_HEIGHT + (2
                            * SEE_FONT_PAD_VERT))
                        ),
                    SEE_FONT_WIDTH  + (2 * SEE_FONT_PAD_HORZ),
                    SEE_FONT_HEIGHT + (2 * SEE_FONT_PAD_VERT),
                    b_color
                );
            }
            for (uint8_t row=0; row<SEE_FONT_HEIGHT; ++row)
            {
                uint8_t row_map = see_font_tbl[ch][row];
                // skip empty rows
                if (row_map != 0)
                {
                    uint8_t mask = 0b10000000;
                    for (uint8_t col=0; col<SEE_FONT_WIDTH; ++col)
                    {
                        if (row_map & mask)
                            vga_driver.vga_put_pixel(cur.x, cur.y, f_color);
                        mask >>= 1;
                        ++cur.x;
                    }
                }
                // reset x, draw next row of font pixels
                cur.x = start.x
                    + (rc_cntr.x * (SEE_FONT_WIDTH + (2 * SEE_FONT_PAD_HORZ)))
                    + SEE_FONT_PAD_HORZ;
                ++cur.y;
            }
            // move right; the character to draw
            ++rc_cntr.x;
            ++str;
        }
    }
}
