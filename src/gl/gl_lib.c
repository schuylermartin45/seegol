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
// byte used to indicate various states in the CXPM encoding
#define CXPM_MARKER    0

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
    vga_driver.vga_vsync = NULL;
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

/*
** Vertical sync control. Useful for slow electron-gun-based displays
*/
void gl_vsync(void)
{
    vga_driver.vga_vsync();
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

/***** Image Draw Functions (driver-independent)     *****/

/*
** Gets information about an image
**
** @param fid File id that identifies the image data to draw from the image
**        file look-up table (users can just simply use a macro)
** @param dims Pointer to a point that will store image size information
**        (i.e. the dimensions; x stores width, y stores height)
**
** @return Color space size of the image
*/
uint8_t gl_img_stat(uint8_t fid, Point_2D* dims)
{
    dims->x = gl_img_tbl[fid][0][0];
    dims->y = gl_img_tbl[fid][0][1];
    // upper 4 bits have transparency information
    return 0x0F & gl_img_tbl[fid][0][2];
}

/*
** Gets transparency information about an image, providing the color code
** representing a transparent portion.
**
** @param fid File id that identifies the image data to draw from the image
**        file look-up table (users can just simply use a macro)
**
** @return 0 if no transparent pixels exist, color code otherwise
*/
uint8_t gl_img_stat_tcode(uint8_t fid)
{
    return (0xF0 & gl_img_tbl[fid][0][2]) >> 4;
}

/*
** Draws a scaled image "installed" on the OS
**
** @param fid File id that identifies the image data to draw from the image
**        file look-up table (users can just simply use a macro)
** @param ul Upper-left starting point
** @param scale Simple (integer) scale factor to make an image larger
**        (duplicates pixels)
*/
void gl_draw_img_scale(uint8_t fid, Point_2D ul, uint8_t scale)
{
    // Phase 0:
    // the file is "read in" from a chunk in memory. It should not be altered
    // by the drawing method; think "read only"
    const uint8_t** fd = gl_img_tbl[fid];

    // Phase 1:
    // file header information: width, height, number of colors
    Point_2D dims;
    uint8_t color_space = gl_img_stat(fid, &dims);
    // transparency color code
    uint8_t t_code = gl_img_stat_tcode(fid);

    // Phase 2:
    // build the bit-map color look-up table, organized as: {key, R, G, B}
    RGB_8 color_map[color_space];
    // value that the table starts at when making look-up values
    // subsequent keys are guaranteed to be consecutive
    uint8_t start_code = fd[1][0];
    // add the colors to the table, based on the header information
    for(int i=1; i<color_space + 1; ++i)
        color_map[fd[i][0] - start_code] = RGB(fd[i][1], fd[i][2], fd[i][3]);

    // Phase 3:
    // go over the image data, dropping pixels
    uint16_t px_cntr = 0;
    uint16_t px_total = dims.x * dims.y;
    Point_2D cursor = {0, 0};
    const uint8_t* byte_arr = fd[color_space + 1];
    // keep track of everything that's been drawn
    while(px_cntr < px_total)
    {
        // determine the action to perform based on a pattern of bytes
        uint8_t byte = *byte_arr++;
        if (byte == CXPM_MARKER)
        {
            // 0XY format: jump cursor to next left region
            cursor.x = *byte_arr++;
            cursor.y = *byte_arr++;
            continue;
        }
        else
        {
            uint8_t encode = byte;
            // region dimensions read from file
            uint16_t w = 1;
            uint16_t h = 1;
            // perform the table look-up, on both pixels
            // upper 4 bits
            uint8_t c0_key = encode >> 4;
            // lower 4 bits
            uint8_t c1_key = encode & 0x0F;
            RGB_8 color0 = color_map[c0_key - start_code];
            RGB_8 color1 = color_map[c1_key - start_code];
            // draw one color code byte
            if (*byte_arr == CXPM_MARKER)
            {
                ++byte_arr;
            }
            // draw region
            else
            {
                w = *byte_arr++;
                h = *byte_arr++;
                // jump if a region has already been painted
                cursor.x = *byte_arr++;
            }
            // draw larger region if codes match and check for transparency
            if (c0_key == c1_key)
            {
                uint16_t draw_w = 2 * w * scale;
                uint16_t draw_h = h * scale;
                // bounds checking
                // TODO this overflows???!!!
                if ((2 * cursor.x * scale) + ul.x + draw_w > gl_getw())
                    draw_w = gl_getw() - ((2 * cursor.x * scale) + ul.x);
                if ((cursor.y * scale) + ul.y + draw_h > gl_geth())
                    draw_h = gl_geth() - ((cursor.y * scale) + ul.y);
                if (c0_key != t_code)
                {
                    // draw with fast rectangles when possible; when both
                    // colors are the same
                    vga_driver.vga_draw_rect_wh(
                        (2 * cursor.x * scale) + ul.x,
                        (cursor.y * scale) + ul.y,
                        draw_w, draw_h, color0
                    );
                }
            }
            else
            {
/*
                // TODO this
                // bounds checking
                //if ((ul.x + x + scale) > vga_driver.screen_w)
                //    break;
                // check for transparency
                if (c0_key != t_code)
                {
                    // draw the pixels, individually accounting for scale
                    vga_driver.vga_draw_rect_wh(
                        (2 * cursor.x * scale) + ul.x,
                        (cursor.y * scale) + ul.y,
                        w * scale, h * scale, color0
                    );
                }
                // TODO this
                //if ((ul.x + x + (2 * scale)) > vga_driver.screen_w)
                //    break;
                if (c1_key != t_code)
                {
                    vga_driver.vga_draw_rect_wh(
                        (2 * (cursor.x + 1) * scale) + ul.x,
                        (cursor.y * scale) + ul.y,
                        w * scale, h * scale, color1
                    );
                }
*/
                color1.r += 3;
            }
            // advance cursor
            cursor.x += w;
            // increment what we have drawn thus far
            px_cntr += 2 * w * h;
        }
    }
}

/*
** Draws an image "installed" on the OS
**
** @param fid File id that identifies the image data to draw from the image
**        file look-up table (users can just simply use a macro)
** @param ul Upper-left starting point
*/
void gl_draw_img(uint8_t fid, Point_2D ul)
{
    gl_draw_img_scale(fid, ul, 1);
}

/*
** Draws a scaled image "installed" on the OS, auto-centered on the screen.
** If the image is scaled too much and goes off the screen, zoom in on the
** upper left corner of the image.
**
** @param fid File id that identifies the image data to draw from the image
**        file look-up table (users can just simply use a macro)
** @param scale Simple (integer) scale factor to make an image larger
**        (duplicates pixels)
*/
void gl_draw_img_center_scale(uint8_t fid, uint8_t scale)
{
    // perform centering calculations by getting size info on the image
    Point_2D ul;
    gl_img_stat(fid, &ul);
    uint16_t x_scale = ul.x * scale;
    uint16_t y_scale = ul.y * scale;
    uint16_t w = gl_getw();
    uint16_t h = gl_geth();
    ul.x = (w > x_scale) ? (w - x_scale) / 2 : 0;
    ul.y = (h > y_scale) ? (h - y_scale) / 2 : 0;
    // calculate the upper left corner that will center the image on the screen
    gl_draw_img_scale(fid, ul, scale);
}

/*
** Draws an image "installed" on the OS, auto-centered on the screen
** If the image is scaled too much and goes off the screen, zoom in on the
** upper left corner of the image.
**
** @param fid File id that identifies the image data to draw from the image
**        file look-up table (users can just simply use a macro)
*/
void gl_draw_img_center(uint8_t fid)
{
    // calculate the upper left corner that will center the image on the screen
    gl_draw_img_center_scale(fid, 1);
}
