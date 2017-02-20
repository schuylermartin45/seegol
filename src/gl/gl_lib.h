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
// text controller
#include "../kern/kio.h"
// VGA mode drivers
#include "../kern/vga/vga.h"
#include "../kern/vga/vga13.h"
// defines the font used to draw strings
#include "see_font.h"

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

/** Functions  **/

/*
** Initialize the graphics library with default values. User should call this
** once per program
*/
void gl_init(void);

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
** Get the current width of the screen
**
** @param Width of the screen in the current mode
*/
uint16_t gl_getw(void);

/*
** Get the current height of the screen
**
** @param Height of the screen in the current mode
*/
uint16_t gl_geth(void);

/***** "Fast" Draw Functions (implemented in driver) *****/

/*
** Clears the video buffer
*/
void gl_clrscr(void);

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

/***** Generic Draw Functions (implemented in GL *****/

/*
** Draws a string, based on a custom-made bitmap font
**
** @param start Starting point to draw the string (upper left pixel)
** @param b_color Background color of the text
** @param f_color Foreground color of the text
** @param str String to draw
*/
void gl_draw_str(Point_2D start, RGB_8 b_color, RGB_8 f_color, char* str);

#endif
