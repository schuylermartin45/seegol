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
// byte used to indicate run-length endcoded section in the CXPM decoder
#define CXPM_MARKER    0

// structure that manages the driver mode currently activated
// assumed to be text mode if graphics haven't been initialized yet
static VGA_Driver vga_driver = {
    TEXT_WIDTH,
    TEXT_HEIGHT,
    (uint16_t*)TEXT_MEM_BEGIN,
    VGA_MODE_TEXT,
    // set all the functions here; shouldn't be called in text mode so use
    // NULL for now
    NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

/************************** Internal Functions *************************/

/************************** User Functions    **************************/

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
    // reset to the default TEXT mode
    vga_driver.screen_w = TEXT_WIDTH;
    vga_driver.screen_h = TEXT_HEIGHT;
    vga_driver.frame_buff = (uint16_t*)TEXT_MEM_BEGIN;
    vga_driver.vga_mode = VGA_MODE_TEXT;
    vga_driver.vga_enter = NULL;
    vga_driver.vga_clrscr = NULL;
    vga_driver.vga_vsync = NULL;
    vga_driver.vga_put_pixel = NULL;
    vga_driver.vga_get_pixel = NULL;
    vga_driver.vga_draw_rect = NULL;
    vga_driver.vga_draw_rect_wh = NULL;
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
** Get the current graphics mode
**
** @return The current graphics mode
*/
uint16_t gl_get_mode(void)
{
    return vga_driver.vga_mode;
}

/*
** Get the current width of the screen
**
** @return Width of the screen in the current mode
*/
uint16_t gl_getw(void)
{
    return vga_driver.screen_w;
}

/*
** Get the current height of the screen
**
** @return Height of the screen in the current mode
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

/***** String Draw Functions (driver-independent)    *****/

/*
** Calculates the bounding box of a string to draw from the bit-mapped SeeFont
**
** @param ul Upper-left starting point
** @param str String to draw
** @param scale Font scale factor (Ex: scale=2: 1 font pixel -> 4 real pixels)
** @param w_bound Right-handed width to bound the text to. This is the maximum 
**        width value that can be drawn. After this word-wrapping is enforced
** @param bb Point to store bounding box width and height information into
*/
void gl_draw_str_bb(Point_2D ul, char* str, uint8_t scale, uint16_t w_bound,
    Point_2D* bb)
{
    // enforce some limit on font scaling
    if ((scale < 1) || (scale > 127))
        scale = 1;
    // clear the bounding box so additions can be made later
    bb->x = 0; bb->y = 0;
    // current position to draw
    Point_2D cur = ul;
    // keeps character row, col counters as we move along
    Point_2D rc_cntr = {0, 0};
    // pad vertically
    cur.y += SEE_FONT_PAD_VERT;
    bb->y += (scale * SEE_FONT_HEIGHT) + (2 * SEE_FONT_PAD_VERT);
    // track the longest run of characters to calculate the max bb width
    uint16_t ch_w_cntr = 0;
    // loop until null byte
    while(*str != '\0')
    {
        // encounter a newline, jump ahead to the next line
        if (*str == '\n')
        {
            if (rc_cntr.x > ch_w_cntr)
                ch_w_cntr = rc_cntr.x;
            // jump a row of chars down
            rc_cntr.x = 0;
            ++rc_cntr.y;
            ++str;
            // increase height of the bounding box
            bb->y += (scale * SEE_FONT_HEIGHT) + (2 * SEE_FONT_PAD_VERT);
        }
        else
        {
            // enforce a simple, hacky, word-wrapping
            // this works by checking how long the next word is after a space
            // has been seen
            uint16_t wrap_cntr = 0;
            if (*str == ' ')
            {
                char* wrap_ptr = str;
                ++wrap_ptr;
                while ((*wrap_ptr != '\0') && (*wrap_ptr != ' '))
                {
                    ++wrap_cntr;
                    ++wrap_ptr;
                }
            }
            // enforce a newline if we are about to go out of bounds, in x
            cur.x = ul.x
                + (rc_cntr.x * ((scale * SEE_FONT_WIDTH) 
                    + (2 * SEE_FONT_PAD_HORZ)))
                + SEE_FONT_PAD_HORZ;
            if ((cur.x + (scale * SEE_FONT_WIDTH)
                + (3 * SEE_FONT_PAD_HORZ)
                + (wrap_cntr * scale * SEE_FONT_WIDTH)) >= w_bound)
            {
                if (*str == ' ')
                    ++str;
                if (rc_cntr.x > ch_w_cntr)
                    ch_w_cntr = rc_cntr.x;
                // reset before draw
                rc_cntr.x = 0;
                ++rc_cntr.y;
                cur.x = ul.x + SEE_FONT_PAD_HORZ;
                // increase height of the bounding box
                bb->y += (scale * SEE_FONT_HEIGHT) + (2 * SEE_FONT_PAD_VERT);
            }
            cur.y = ul.y
                + (rc_cntr.y * ((scale * SEE_FONT_HEIGHT)
                + (2 * SEE_FONT_PAD_VERT)))
                + SEE_FONT_PAD_VERT;
            if (rc_cntr.x > ch_w_cntr)
                ch_w_cntr = rc_cntr.x;
            // move right; the character to draw
            ++rc_cntr.x;
            ++str;
        }
    }
    // calculate the bounding box width
    bb->x = ch_w_cntr * ((scale * SEE_FONT_WIDTH) + (2 * SEE_FONT_PAD_HORZ));
}

/*
** Draws a string, based on a custom-made bitmap font.
** "Transparent" backgrounds are achieved by setting the background and
** foreground colors to the same value. Includes font scaling and the ability
** to enforce a width boundary to word-wrap on.
**
** @param ul Upper-left starting point
** @param b_color Background color of the text
** @param f_color Foreground color of the text
** @param str String to draw
** @param scale Font scale factor (Ex: scale=2: 1 font pixel -> 4 real pixels)
** @param w_bound Right-handed width to bound the text to. This is the maximum 
**        width value that can be drawn. After this word-wrapping is enforced
*/
void gl_draw_str_scale(Point_2D ul, RGB_8 b_color, RGB_8 f_color, char* str,
    uint8_t scale, uint16_t w_bound)
{
    // enforce some limit on font scaling
    if ((scale < 1) || (scale > 127))
        scale = 1;
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
            // enforce a simple, hacky, word-wrapping
            // this works by checking how long the next word is after a space
            // has been seen
            uint16_t wrap_cntr = 0;
            if (ch == ' ')
            {
                char* wrap_ptr = str;
                ++wrap_ptr;
                while ((*wrap_ptr != '\0') && (*wrap_ptr != ' '))
                {
                    ++wrap_cntr;
                    ++wrap_ptr;
                }
            }
            // enforce a newline if we are about to go out of bounds, in x
            cur.x = ul.x
                + (rc_cntr.x * ((scale * SEE_FONT_WIDTH) 
                    + (2 * SEE_FONT_PAD_HORZ)))
                + SEE_FONT_PAD_HORZ;
            if ((cur.x + (scale * SEE_FONT_WIDTH)
                + (3 * SEE_FONT_PAD_HORZ)
                + (wrap_cntr * scale * SEE_FONT_WIDTH)) >= w_bound)
            {
                if (ch == ' ')
                {
                    ++str;
                    ch = *str;
                }
                // reset before draw
                rc_cntr.x = 0;
                ++rc_cntr.y;
                cur.x = ul.x + SEE_FONT_PAD_HORZ;
            }
            cur.y = ul.y
                + (rc_cntr.y * ((scale * SEE_FONT_HEIGHT)
                + (2 * SEE_FONT_PAD_VERT)))
                + SEE_FONT_PAD_VERT;
            // check for transparent backgrounds; both colors are the same
            if (!vga_RGB_8_cmp(b_color, f_color))
            {
                // quickly draw the background color using the rectangle draw
                // function and use this assumption for other optimizations
                vga_driver.vga_draw_rect_wh(
                    ul.x + 
                        (rc_cntr.x * ((scale * SEE_FONT_WIDTH)
                            + (2 * SEE_FONT_PAD_HORZ))
                        ),
                    ul.y +
                        (rc_cntr.y * ((scale * SEE_FONT_HEIGHT)
                            + (2 * SEE_FONT_PAD_VERT))
                        ),
                    (scale * SEE_FONT_WIDTH)  + (2 * SEE_FONT_PAD_HORZ),
                    (scale * SEE_FONT_HEIGHT) + (2 * SEE_FONT_PAD_VERT),
                    b_color
                );
            }
            for(uint8_t row=0; row<SEE_FONT_HEIGHT; ++row)
            {
                uint8_t row_map = see_font_tbl[ch - SEE_FONT_START_CH][row];
                // skip empty rows
                if (row_map != 0)
                {
                    uint8_t mask = 0b10000000;
                    for(uint8_t col=0; col<SEE_FONT_WIDTH; ++col)
                    {
                        if (row_map & mask)
                        {
                            vga_driver.vga_draw_rect_wh(cur.x, cur.y,
                                scale, scale, f_color);
                        }
                        mask >>= 1;
                        cur.x += scale;
                    }
                }
                // reset x, draw next row of font pixels
                cur.x = ul.x
                    + (rc_cntr.x *
                        (scale * SEE_FONT_WIDTH) + (2 * SEE_FONT_PAD_HORZ))
                    + SEE_FONT_PAD_HORZ;
                cur.y += scale;
            }
            // move right; the character to draw
            ++rc_cntr.x;
            ++str;
        }
    }
}

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
    gl_draw_str_scale(ul, b_color, f_color, str, 1, vga_driver.screen_w);
}

/*
** Calculates the bounding box of a string to draw from the bit-mapped SeeFont
** using kio_sprintf
**
** @param ul Upper-left starting point
** @param str String to draw
** @param scale Font scale factor (Ex: scale=2: 1 font pixel -> 4 real pixels)
** @param w_bound Right-handed width to bound the text to. This is the maximum 
**        width value that can be drawn. After this word-wrapping is enforced
** @param bb Point to store bounding box width and height information into
** @param a0 First arugment to print
** @param a1 Second arugment to print
*/
void gl_draw_strf_bb(Point_2D ul, char* str, uint8_t scale, uint16_t w_bound,
    Point_2D* bb, void* a0, void* a1)
{
    // autosize buffer
    uint16_t size = kio_sprintf_len(str, a0, a1);
    char buff[size];
    // call sprintf, then dump to the screen
    kio_sprintf(str, buff, a0, a1);
    // now calculate the bounding box
    gl_draw_str_bb(ul, buff, scale, w_bound, bb);
}

/*
** Draws a string using kio_sprintf, based on a custom-made bitmap font.
** "Transparent" backgrounds are achieved by setting the background and
** foreground colors to the same value. Includes font scaling.
**
** @param ul Upper-left starting point
** @param b_color Background color of the text
** @param f_color Foreground color of the text
** @param str String to draw
** @param scale Font scale factor (Ex: scale=2: 1 font pixel -> 4 real pixels)
** @param w_bound Right-handed width to bound the text to. This is the maximum 
**        width value that can be drawn. After this word-wrapping is enforced
** @param a0 First arugment to print
** @param a1 Second arugment to print
*/
void gl_draw_strf_scale(Point_2D ul, RGB_8 b_color, RGB_8 f_color, char* str,
    uint8_t scale, uint16_t w_bound, void* a0, void* a1)
{
    // autosize buffer
    uint16_t size = kio_sprintf_len(str, a0, a1);
    char buff[size];
    // call sprintf, then dump to the screen
    kio_sprintf(str, buff, a0, a1);
    gl_draw_str_scale(ul, b_color, f_color, buff, scale, w_bound);
}

/*
** Draws a string using kio_sprintf, based on a custom-made bitmap font.
** "Transparent" backgrounds are achieved by setting the background and
** foreground colors to the same value.
**
** @param ul Upper-left starting point
** @param b_color Background color of the text
** @param f_color Foreground color of the text
** @param str String to draw
** @param a0 First arugment to print
** @param a1 Second arugment to print
*/
void gl_draw_strf(Point_2D ul, RGB_8 b_color, RGB_8 f_color, char* str,
    void* a0, void* a1)
{
    gl_draw_strf_scale(ul, b_color, f_color, str, 1, vga_driver.screen_w,
        a0, a1);
}

/***** Image Draw Functions (driver-independent)     *****/

/*
** Gets information about an image
**
** @param fid File id that identifies the image data to draw from the image
**        file look-up table (users can just simply use a macro)
** @param dims Pointer to a point that will store image size information
**        (i.e. the dimensions; x stores width, y stores height)
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
    for(uint16_t y=0; y<dims.y; ++y)
    {
        // skip drawing scanlines that are out-of-bounds
        if (ul.y + (y * scale) > vga_driver.screen_h)
            break;
        // x determines if we have reached the end of the image
        uint16_t x = 0;
        // x_b is they byte position on the line
        uint16_t x_b = 0;
        // run until the end of the scanline or terminate if we go off the 
        // screen
        while((x < (dims.x * scale)) && ((ul.x + x) <= vga_driver.screen_w))
        {
            // read in the color encoding; advancing to the next encoded
            // position in the process
            uint8_t encode = fd[y + color_space + 1][x_b++];
            // decode run-length encoding
            // otherwise, draw two pixels at once, one time
            uint16_t run_len = 1;
            if (encode == CXPM_MARKER)
            {
                // decode the next byte as the run length
                run_len = fd[y + color_space + 1][x_b++];
                // then this byte is the color key to draw
                encode = fd[y + color_space + 1][x_b++];
            }
            // perform the table look-up, on both pixels
            // upper 4 bits
            uint8_t c0_key = encode >> 4;
            // lower 4 bits
            uint8_t c1_key = encode & 0x0F;
            RGB_8 color0 = color_map[c0_key - start_code];
            RGB_8 color1 = color_map[c1_key - start_code];
            // scale, taking account for the fact that 2 pixels are drawn at
            // one time
            uint16_t px_scale = 2 * scale;
            uint16_t draw_w = run_len * px_scale;
            uint16_t draw_h = scale;
            // draw until far edge
            if ((ul.x + x + draw_w) > vga_driver.screen_w)
                draw_w = vga_driver.screen_w - (ul.x + x);
            // draw larger region if codes match and check for transparency
            if (c0_key == c1_key)
            {
                if (c0_key != t_code)
                {
                    // draw with fast rectangles when possible; when both
                    // colors are the same
                    vga_driver.vga_draw_rect_wh(
                        ul.x + x, ul.y + (y * scale),
                        draw_w, draw_h, color0
                    );
                }
            }
            else
            {
                for(uint16_t i=0; i<run_len; ++i)
                {
                    // bounds checking
                    if ((ul.x + x + scale) > vga_driver.screen_w)
                        break;
                    // check for transparency
                    if (c0_key != t_code)
                    {
                        // draw the pixels, individually accounting for scale
                        vga_driver.vga_draw_rect_wh(
                            ul.x + x, ul.y + (y * scale),
                            scale, draw_h, color0
                        );
                    }
                    if ((ul.x + x + (2 * scale)) > vga_driver.screen_w)
                        break;
                    if (c1_key != t_code)
                    {
                        vga_driver.vga_draw_rect_wh(
                            ul.x + x + scale, ul.y + (y * scale),
                            scale, draw_h, color1
                        );
                    }
                }
            }
            x += run_len * px_scale;
        }
    }
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

/***** Line Draw Functions (driver-independent)      *****/

/*
** Draw a line anywhere on the screen
**
** @param p0 First point
** @param p1 Second point
** @param width Line width/thickness
** @param color Color to draw
*/
void gl_draw_line_width(Point_2D p0, Point_2D p1, uint8_t width, RGB_8 color)
{
    // start by performing the left-right coordinate check-and-swap
    if (p0.x > p1.x)
    {
        Point_2D tp = p0;
        p0 = p1; p1 = tp;
    }
    // bound points to within the draw space
    if (p0.x > vga_driver.screen_w)
        p0.x = vga_driver.screen_w;
    if (p1.x > vga_driver.screen_w)
        p1.x = vga_driver.screen_w;
    if (p0.y > vga_driver.screen_h)
        p0.y = vga_driver.screen_h;
    if (p1.y > vga_driver.screen_h)
        p1.y = vga_driver.screen_h;

    // check for optimized line drawing
    if (p0.y == p1.y)
        vga_driver.vga_draw_rect_wh(p0.x, p0.y, p1.x-p0.x, width, color);
    else if (p0.x == p1.x)
    {
        // perform the "left-right" swapping of the coordinates but for drawing
        // vertical lines (ensure that p1-p0 is positive)
        if (p0.y > p1.y)
        {
            Point_2D tp = p0;
            p0 = p1, p1=tp;
        }
        vga_driver.vga_draw_rect_wh(p0.x, p0.y, width, p1.y-p0.y, color);
    }
    else
    {
        // calculate slope component for each direction
        int16_t dx = p1.x - p0.x, dy = p1.y - p0.y;
        // draw diagonal lines if slope is +/-1
        if ((dx == dy) || (dx == -dy))
        {
            for (uint16_t i=0; i<=(p1.x - p0.x); ++i)
            {
                // negative slopes
                if (p0.y < p1.y)
                    // TODO
                    //vga_driver.vga_put_pixel(p0.x + i, p0.y + i, color);
                    vga_driver.vga_draw_rect_wh(p0.x + i, p0.y + i, width, 1,
                        color);
                // positive slopes
                else
                    // TODO
                    //vga_driver.vga_put_pixel(p0.x + i, p0.y - i, color);
                    vga_driver.vga_draw_rect_wh(p0.x + i, p0.y - i, width, 1,
                        color);
            }
        }
        // draw...everything else (in the octants)
        else
        {
            // set x and y to the starting point/pixel; these then change with
            // each iteration of the drawing
            int16_t x = p0.x, y = p0.y;
            // choose which direction (+/-) to increment x or y in
            int16_t incr_x = 1, incr_y = (dy > 0) ? 1 : -1;

            int16_t abs_dy = (dy < 0) ? -dy : dy;
            // steep lines iterate over y instead of x (occurs when |dy| > dx)
            // **Note: dx will always be + as we always draw left-to-right
            // establish deltas for next movement
            int16_t dE = 2 * dx;
            int16_t dNE = (dy > 0) ? 2 * (dx - dy) : 2 * (dx + dy);
            // initial delta value
            int16_t delta = (dy > 0) ? dE - dy : dE + dy;
            if (abs_dy > dx)
            {
                // as dy can be + or -, >= or <= checks aren't going to
                // cut it. So instead we will loop until we ~ reach p1...(*)
                for (; y != p1.y; y += incr_y)
                {
                    // color and pick the next pixel
                    // TODO
                    //vga_driver.vga_put_pixel(x, y, color);
                    vga_driver.vga_draw_rect_wh(x, y, width, 1, color);
                    if (delta <= 0)
                        delta += dE;
                    else
                    {
                        x += incr_x;
                        delta += dNE;
                    }
                }
                // (*)... and then draw the last point at p1
                vga_driver.vga_put_pixel(x, y, color);
            }
            // draw gradual-sloped  lines (first and last octant)
            // this works similar to the code above but is a bit simpler as dx
            // will always be + because we gaurantee left-to-right coordinates
            else
            {
                // establish deltas for next movement
                dE = (dy > 0) ? 2 * dy : -2 * dy;
                dNE = (dy > 0) ? 2 * (dy - dx) : 2 * (-dy - dx);
                // initial delta value
                delta = dE - dx;
                for (; x <= p1.x; x += incr_x)
                {
                    // color and pick the next pixel
                    // TODO
                    //vga_driver.vga_put_pixel(x, y, color);
                    vga_driver.vga_draw_rect_wh(x, y, 1, width, color);
                    if (delta <= 0)
                        delta += dE;
                    else
                    {
                        y += incr_y;
                        delta += dNE;
                    }
                }
            }
        }
    }
}
