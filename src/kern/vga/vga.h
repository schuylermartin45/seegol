/*
** File:    vga.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Common VGA definitions and utilities
*/
#ifndef _VGA_H_
#define _VGA_H_

/** Headers    **/
#include "gcc16.h"
#include "types.h"

/** Macros     **/
// identifiers for the various graphics modes
#define VGA_MODE_TEXT    0x03
#define VGA_MODE_13      0x13

/** Structures **/
// RGB color systems
typedef struct RGB_8
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB_8;

// defines a standard set of operations for VGA drivers
typedef struct VGA_Driver VGA_Driver;
struct VGA_Driver
{
    // current screen size
    uint16_t screen_w;
    uint16_t screen_h;
    // address of start of the frame buffer for each mode
    uint16_t* frame_buff;
    // remember what graphical mode we are in
    uint8_t vga_mode;

    /*
    ** Start a VGA mode
    **
    ** @param driver Standard driver spec for a driver to setup
    ** @param aux Auxiliary input data. This is mode specific
    */
    void (*vga_enter)(VGA_Driver* driver, uint16_t aux);

    /*
    ** Clears the video buffer
    */
    void (*vga_clrscr)(void);


    /*
    ** Write a byte out to the frame buffer. This represents a single pixel
    **
    ** @param addr Address of the pixel in the frame buffer to write to
    ** @param pxb Pixel byte to write
    */
    void (*vga_fbwb)(uint8_t* addr, uint8_t pxb);

    /*
    ** Read a byte out of the frame buffer. This represents a single pixel
    **
    ** @param addr Address of the pixel in the frame buffer to write to
    ** @return Pixel's color value
    */
    uint8_t (*vga_fbrb)(uint8_t* addr);

    /*
    ** Write a pixel out to the frame buffer. This represents a single pixel
    **
    ** @param x coordinate on the screen
    ** @param y coordinate on the screen
    ** @param color Pixel color to write
    */
    void (*vga_put_pixel)(uint16_t x, uint16_t y, RGB_8* color);

    /*
    ** Read a pixel out of the frame buffer. This represents a single pixel
    **
    ** @param x coordinate on the screen
    ** @param y coordinate on the screen
    ** @return Pixel color at position
    */
    uint8_t (*vga_get_pixel)(uint16_t x, uint16_t y);

    /*
    ** Draws a simple rectangle
    **
    ** @param urx Upper-right x coordinate on the screen
    ** @param ury Upper-right y coordinate on the screen
    ** @param llx Lower-left x coordinate on the screen
    ** @param lly Lower-left y coordinate on the screen
    ** @param color Pixel color to write
    */
    void (*vga_draw_rect)(uint16_t urx, uint16_t ury, uint16_t llx,
        uint16_t lly, RGB_8* color);

    /*
    ** Draws a simple rectangle, using alternative parameter listings
    **
    ** @param ulx Upper-left x coordinate on the screen
    ** @param uly Upper-left y coordinate on the screen
    ** @param w Width of the rectangle
    ** @param h Height of the rectangle
    ** @param color Pixel color to write
    */
    void (*vga_draw_rect_wh)(uint16_t ulx, uint16_t uly, uint16_t w,
        uint16_t h, RGB_8* color);

};

/** Globals    **/
// common colors
extern RGB_8 RGB_8_BLACK;
extern RGB_8 RGB_8_WHITE;
extern RGB_8 RGB_8_HSC;

/** Functions  **/

/*
** Compares two colors for equivalency
**
** @param c0 First color
** @param c1 Second color
** @param Color equivalency
*/
bool vga_RGB_8_cmp(RGB_8* c0, RGB_8* c1);

#endif