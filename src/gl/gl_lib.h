/*
** File:    gl_lib.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: User-level graphics library. The default is "text mode" and
**              this library handles some of the switching code that KIO
**              doesn't need to know about (i.e. using BIOS interrupts to
**              switch modes). Otherwise, GL more or less wraps around the VGA
**              driver function calls.
*/
#ifndef _GL_LIB_H_
#define _GL_LIB_H_

/** Headers    **/
#include "../kern/gcc16.h"
#include "../kern/types.h"
// vga.h defines RGB_8
#include "../kern/vga/vga.h"

/** Globals    **/

/** Structures **/

// Point systems
typedef struct Point_2D
{
    uint16_t x;
    uint16_t y;
} Point_2D;
typedef struct Point_3D
{
    uint16_t x;
    uint16_t y;
    uint16_t z;
} Point_3D;

/** Macros     **/
// short-hand, "in-place" initializers
#define PT2(X, Y)       (Point_2D){X, Y}
#define PT3(X, Y, Z)    (Point_3D){X, Y, Z}
// RGB is defined by vga.h for convience
#define RGB(R, G, B)    (RGB_8){R, G, B}

// common colors, available to user programs
#define RGB_BLACK       RGB(  0,   0,   0)
#define RGB_WHITE       RGB(255, 255, 255)
#define RGB_HSC         RGB( 40,  49, 137)
// subtractive colors
#define RGB_CYAN         RGB(  0, 255, 255)
#define RGB_MAGENTA      RGB(255,   0, 255)
#define RGB_YELLOW       RGB(255, 255,   0)

/** Functions  **/

/*
** Enter a graphical mode
**
** @param mode Graphics mode to select
*/
void gl_enter(uint8_t mode);

/*
** Exit graphical mode and go back to text
*/
void gl_exit(void);

/*
** Get the current graphics mode
**
** @return The current graphics mode
*/
uint16_t gl_get_mode(void);

/*
** Get the current width of the screen
**
** @return Width of the screen in the current mode
*/
uint16_t gl_getw(void);

/*
** Get the current height of the screen
**
** @return Height of the screen in the current mode
*/
uint16_t gl_geth(void);

/***** "Fast" Draw Functions (implemented in driver) *****/

/*
** Clears the video buffer
*/
void gl_clrscr(void);

/*
** Vertical sync control. Useful for slow electron-gun-based displays
*/
void gl_vsync(void);

/*
** Draws a pixel to the screen. Coordinates start in the upper-left corner
**
** @param pt Pixel position
** @param color Pixel color to write. This is an index into the color palette
*/
void gl_put_pixel(Point_2D pt, RGB_8 color);

/*
** Gets a pixel's color value. Coordinates start in the upper-left corner
**
** @param pt Pixel position
** @param color Pixel color written to the pixel position
*/
void gl_get_pixel(Point_2D pt, RGB_8* color);

/*
** Draws a simple rectangle
**
** @param ur Upper-right coordinate on the screen
** @param ll Lower-left coordinate on the screen
** @param color Pixel color to write. This is an index into the color palette
*/
void gl_draw_rect(Point_2D ur, Point_2D ll, RGB_8 color);

/*
** Draws a simple rectangle, using alternative parameter listings
**
** @param ul Upper-left coordinate on the screen
** @param w Width of the rectangle
** @param h Height of the rectangle
** @param color Pixel color to write. This is an index into the color palette
*/
void gl_draw_rect_wh(Point_2D ul, uint16_t w, uint16_t h, RGB_8 color);

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
    Point_2D* bb);

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
    uint8_t scale, uint16_t w_bound);

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
void gl_draw_str(Point_2D start, RGB_8 b_color, RGB_8 f_color, char* str);

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
    Point_2D* bb, void* a0, void* a1);

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
    uint8_t scale, uint16_t w_bound, void* a0, void* a1);

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
    void* a0, void* a1);

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
uint8_t gl_img_stat(uint8_t fid, Point_2D* dims);

/*
** Gets transparency information about an image, providing the color code
** representing a transparent portion.
**
** @param fid File id that identifies the image data to draw from the image
**        file look-up table (users can just simply use a macro)
** @return 0 if no transparent pixels exist, color code otherwise
*/
uint8_t gl_img_stat_tcode(uint8_t fid);

/*
** Draws a scaled image "installed" on the OS
**
** @param fid File id that identifies the image data to draw from the image
**        file look-up table (users can just simply use a macro)
** @param ul Upper-left starting point
** @param scale Simple (integer) scale factor to make an image larger
**        (duplicates pixels)
*/
void gl_draw_img_scale(uint8_t fid, Point_2D ul, uint8_t scale);

/*
** Draws an image "installed" on the OS
**
** @param fid File id that identifies the image data to draw from the image
**        file look-up table (users can just simply use a macro)
** @param ul Upper-left starting point
*/
#define gl_draw_img(fid, ul) gl_draw_img_scale(fid, Point_2D ul, 1)

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
void gl_draw_img_center_scale(uint8_t fid, uint8_t scale);

/*
** Draws an image "installed" on the OS, auto-centered on the screen.
** If the image is scaled too much and goes off the screen, zoom in on the
** upper left corner of the image.
**
** @param fid File id that identifies the image data to draw from the image
**        file look-up table (users can just simply use a macro)
*/
#define gl_draw_img_center(fid) gl_draw_img_center_scale(fid, 1)

/***** Line Draw Functions (driver-independent)      *****/

/*
** Draw a line anywhere on the screen
**
** @param p0 First point
** @param p1 Second point
** @param width Line width/thickness
** @param color Color to draw
*/
void gl_draw_line_width(Point_2D p0, Point_2D p1, uint8_t width, RGB_8 color);

/*
** Draw a line anywhere on the screen
**
** @param p0 First point
** @param p1 Second point
** @param color Color to draw
*/
#define gl_draw_line(p0, p1, color) gl_draw_line_width(p0, p1, 1, color)

#endif
