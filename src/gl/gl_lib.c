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

#include "../kern/debug.h"
#include "../kern/kio.h"
// VGA mode drivers
#include "../kern/vga/vga13.h"

// Includes only needed local to the C file and don't need to be included
// everywhere else (especially since these ones are fairly large)
// image data array
#include "img_tbl.h"
// defines the font used to draw strings
#include "see_font.h"

/** Macros     **/
// buffer size to read digits out of the header string
#define CXPM_HEAD_BUFF_SIZE 5
// Base used to encode run length distances in the CXPM format
#define CXPM_RL_BASE        36
// byte used to indicate Huffman endcoded section in the CXPM decoder
#define CXPM_HUFF_MARKER    1

// structure that manages the driver mode currently activated
// assumed to be text mode if graphics haven't been initialized yet
static VGA_Driver vga_driver;

/************************** Internal Functions *************************/

/************************** User Functions    **************************/

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
** @param ul Upper-left starting point
** @param b_color Background color of the text
** @param f_color Foreground color of the text
** @param str String to draw
*/
void gl_draw_str(Point_2D ul, RGB_8 b_color, RGB_8 f_color, char* str)
{
    // current position to draw
    Point_2D cur = ul;
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
            cur.x = ul.x
                + (rc_cntr.x * (SEE_FONT_WIDTH + (2 * SEE_FONT_PAD_HORZ)))
                + SEE_FONT_PAD_HORZ;
            if ((cur.x + SEE_FONT_WIDTH + (3 * SEE_FONT_PAD_HORZ)) 
                >= vga_driver.screen_w)
            {
                // reset before draw
                rc_cntr.x = 0;
                ++rc_cntr.y;
                cur.x = ul.x + SEE_FONT_PAD_HORZ;
            }
            cur.y = ul.y
                + (rc_cntr.y * (SEE_FONT_HEIGHT + (2 * SEE_FONT_PAD_VERT)))
                + SEE_FONT_PAD_VERT;
            // check for transparent backgrounds; both colors are the same
            if (!vga_RGB_8_cmp(b_color, f_color))
            {
                // quickly draw the background color using the rectangle draw
                // function and use this assumption for other optimizations
                vga_driver.vga_draw_rect_wh(
                    ul.x + 
                        (rc_cntr.x * (SEE_FONT_WIDTH + (2
                            * SEE_FONT_PAD_HORZ))
                        ),
                    ul.y +
                        (rc_cntr.y * (SEE_FONT_HEIGHT + (2
                            * SEE_FONT_PAD_VERT))
                        ),
                    SEE_FONT_WIDTH  + (2 * SEE_FONT_PAD_HORZ),
                    SEE_FONT_HEIGHT + (2 * SEE_FONT_PAD_VERT),
                    b_color
                );
            }
            for(uint8_t row=0; row<SEE_FONT_HEIGHT; ++row)
            {
                uint8_t row_map = see_font_tbl[ch][row];
                // skip empty rows
                if (row_map != 0)
                {
                    uint8_t mask = 0b10000000;
                    for(uint8_t col=0; col<SEE_FONT_WIDTH; ++col)
                    {
                        if (row_map & mask)
                            vga_driver.vga_put_pixel(cur.x, cur.y, f_color);
                        mask >>= 1;
                        ++cur.x;
                    }
                }
                // reset x, draw next row of font pixels
                cur.x = ul.x
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

/*
** Draws a scaled image "installed" on the OS
**
** @param ul Upper-left starting point
** @param fid File id that identifies the image data to draw from the image
**        file look-up table (users can just simply use a macro)
** @param scale Simple (integer) scale factor to make an image larger
**        (duplicates pixels)
*/
void gl_draw_img_scale(Point_2D ul, uint8_t fid, uint8_t scale)
{
    // Phase 0:
    // the file is "read in" from a chunk in memory. It should not be altered
    // by the drawing method; think "read only"
    char** fd = gl_img_tbl[fid];

    // Phase 1:
    // file header information: width, height, number of colors
    uint16_t w = 0, h = 0, color_space = 0;
    // read 4 numbers from the file header
    uint8_t val_cntr = 0;
    char head_buff[CXPM_HEAD_BUFF_SIZE];
    for (uint8_t i=0; i<CXPM_HEAD_BUFF_SIZE; ++i)
        head_buff[i] = '\0';
    char* head_ptr = head_buff;
    char* head_cur = fd[0];
    while(*head_cur != '\0')
    {
        // we found the end of a number, convert the string to a number
        if (*head_cur == ' ')
        {
            uint16_t val = kio_str_int(head_buff, 10);
            switch (val_cntr)
            {
                case 0: w = val;
                    break;
                case 1: h = val;
                    break;
            }
            // advance to the next number and clear the buffer
            ++val_cntr;
            for (uint8_t i=0; i<CXPM_HEAD_BUFF_SIZE; ++i)
                head_buff[i] = '\0';
            head_ptr = head_buff;
        }
        else
            *head_ptr++ = *head_cur;
        ++head_cur;
    }
    // set the last value from the header file
    color_space = kio_str_int(head_buff, 10);
    // TODO image data
    #ifdef DEBUG_MODE
        kio_printf("Image resolution: %dx%d", &w, &h);
        kio_printf(" w/ %d colors\n", &color_space, NULL);
    #endif

    // Phase 2:
    // build the bit-map color look-up table, organized as: <key><R><G><B>
    RGB_8 color_map[color_space];
    // ASCII character that the table starts at when making look-up values
    char ascii_start = fd[1][0];
    for (int i=1; i<color_space + 1; ++i)
    {
        // read the first line and jump to the first hex code
        char* line = fd[i] + 1;
        // convert the hex codes to color bytes
        char hex_buff[3] = {line[0], line[1], '\0'};
        uint8_t r = kio_str_int(hex_buff, 16);
        line += 2;
        hex_buff[0] = line[0]; hex_buff[1] = line[1];
        uint8_t g = kio_str_int(hex_buff, 16);
        line += 2;
        hex_buff[0] = line[0]; hex_buff[1] = line[1];
        uint8_t b = kio_str_int(hex_buff, 16);
        // add the colors to the table; mapping the character to a color in
        // the look-up table
        color_map[fd[i][0] - ascii_start] = RGB(r, b, g);
    }

    // Phase 3:
    // go over the image data, dropping pixels
    for(uint16_t y=0; y<h; ++y)
    {
        // skip drawing scanlines that are out-of-bounds
        if (ul.y + (y * scale) > vga_driver.screen_h)
            break;
        // x determines if we have reached the end of the image
        uint16_t x = 0;
        // x_b is they byte position on the line
        uint16_t x_b = 0;
        // run until the end of the line or terminate if we go off the screen
        while((fd[y + color_space + 1][x_b] != '\0')
            && ((ul.x + x) <= vga_driver.screen_w))
        {
            // read in the color encoding; advancing to the next encoded
            // position in the process
            uint8_t encode = fd[y + color_space + 1][x_b++];
            // decode huffman encoding runs
            // otherwise, draw two pixels at once, one time
            uint16_t run_len = 1;
            if (encode == CXPM_HUFF_MARKER)
            {
                // decode the next byte as the run length
                char rl_buff[2] = {fd[y + color_space + 1][x_b++], '\0'};
                run_len = kio_str_int(rl_buff, CXPM_RL_BASE);
                encode = fd[y + color_space + 1][x_b++];
            }
            // perform the table look-up, on both pixels
            // upper 4 bits
            uint8_t c0_key = (encode >> 4) - ascii_start;
            // lower 4 bits
            uint8_t c1_key = (encode & 0x0F) - ascii_start;
            RGB_8 color0 = color_map[c0_key];
            RGB_8 color1 = color_map[c1_key];
            // scale, taking account for the fact that 2 pixels are drawn at
            // one time
            uint16_t px_scale = 2 * scale;
            uint16_t draw_w = run_len * px_scale;
            uint16_t draw_h = scale;
            // draw until far edge
            if ((ul.x + x + draw_w) > vga_driver.screen_w)
                draw_w = vga_driver.screen_w - (ul.x + x);
            // bounds checking; don't draw past the bottom and left borders
            if (c0_key == c1_key)
            {
                // draw with fast rectangles when possible; when both colors
                // are the same
                vga_driver.vga_draw_rect_wh(
                    ul.x + x, ul.y + (y * scale),
                    draw_w, draw_h, color0
                );
            }
            else
            {
                for(uint16_t i=0; i<run_len; ++i)
                {
                    // bounds checking
                    if ((ul.x + x + scale) > vga_driver.screen_w)
                        break;
                    // draw the pixels, individually accounting for scale
                    vga_driver.vga_draw_rect_wh(
                        ul.x + x, ul.y + (y * scale),
                        scale, draw_h, color0
                    );
                    if ((ul.x + x + (2 * scale)) > vga_driver.screen_w)
                        break;
                    vga_driver.vga_draw_rect_wh(
                        ul.x + x + scale, ul.y + (y * scale),
                        scale, draw_h, color1
                    );
                }
            }
            x += run_len * px_scale;
        }
    }
}

/*
** Draws a scaled image "installed" on the OS
**
** @param ul Upper-left starting point
** @param fid File id that identifies the image data to draw from the image
**        file look-up table (users can just simply use a macro)
*/
void gl_draw_img(Point_2D ul, uint8_t fid)
{
    gl_draw_img_scale(ul, fid, 1);
}
