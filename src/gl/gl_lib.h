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
#include "../kern/vga/vga13.h"

/** Macros     **/
// identifiers for the various graphics modes
#define GL_MODE_TEXT    0x03
#define GL_MODE_VGA13   0x13

/** Globals    **/

/** Structures **/

/** Functions  **/

/*
** Enter a graphical mode
**
** @param mode Graphics mode to select
*/
void gl_lib_enter(uint8_t mode);

/*
** Exit graphical mode and go back to text
*/
void gl_lib_exit(void);

/*
** Get the current width of the screen
**
** @param Width of the screen in the current mode
*/
uint16_t gl_lib_getw(void);

/*
** Get the current height of the screen
**
** @param Height of the screen in the current mode
*/
uint16_t gl_lib_geth(void);

/*
** Clears the video buffer
*/
void gl_lib_clrscr(void);

/*
** Draws a pixel to the screen. Coordinates start in the upper-left corner
**
** @param x Pixel position in the x direction
** @param y Pixel position in the y direction
** @param color Pixel color to write. This is an index into the color palette
*/
void gl_lib_put_pixel(uint16_t x, uint16_t y, uint8_t color);

/*
** Gets a pixel's color value. Coordinates start in the upper-left corner
**
** @param x Pixel position in the x direction
** @param y Pixel position in the y direction
** @return Pixel's color value
*/
uint8_t gl_lib_get_pixel(uint16_t x, uint16_t y);

/*
** Draws a simple rectangle
**
** @param urx Upper-right x coordinate on the screen
** @param ury Upper-right y coordinate on the screen
** @param llx Lower-left x coordinate on the screen
** @param lly Lower-left y coordinate on the screen
** @param color Pixel color to write. This is an index into the color palette
*/
void gl_lib_draw_rect(uint16_t urx, uint16_t ury, uint16_t llx, uint16_t lly, 
    uint8_t color);

/*
** Draws a simple rectangle, using alternative parameter listings
**
** @param ulx Upper-left x coordinate on the screen
** @param uly Upper-left y coordinate on the screen
** @param w Width of the rectangle
** @param h Height of the rectangle
** @param color Pixel color to write. This is an index into the color palette
*/
void gl_lib_draw_rect_wh(uint16_t ulx, uint16_t uly, uint16_t w, uint16_t h,
    uint8_t color);

#endif
