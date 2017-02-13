/*
** File:    vga13.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Driver for VGA's "Mode 13h" graphics mode
*/
#ifndef _VGA13_H_
#define _VGA13_H_

/** Headers    **/
#include "../gcc16.h"
#include "../types.h"

/** Macros     **/
// Start and ending address of the memory space that Mode13h uses (64kB total)
#define VGA13_MEM_SIZE   0xFA00
#define VGA13_MEM_BEGIN  0xA0000
#define VGA13_MEM_END    (VGA13_MEM_BEGIN + VGA13_MEM_SIZE)

// Graphics mode screen dimensions (1 byte per pixel in memory)
#define VGA13_WIDTH     320
#define VGA13_HEIGHT    200

/** Globals    **/

/** Structures **/

/** Functions  **/

/*
** Clears the video buffer
*/
void _vga13_clrscr(void);

/*
** Start VGA's Mode13h
*/
void _vga13_enter(void);

/*
** Write a byte out to the frame buffer. This represents a single pixel
**
** @param addr Address of the pixel in the frame buffer to write to
** @param pxb Pixel byte to write. This is an index into the color palette
*/
void _vga13_fbwb(uint8_t* addr, uint8_t pxb);

/*
** Read a byte out of the frame buffer. This represents a single pixel
**
** @param addr Address of the pixel in the frame buffer to write to
** @return Pixel's color value
*/
uint8_t _vga13_fbrb(uint8_t* addr);

/*
** Write a pixel out to the frame buffer. This represents a single pixel
**
** @param x coordinate on the screen
** @param y coordinate on the screen
** @param color Pixel color to write. This is an index into the color palette
*/
void _vga13_put_pixel(uint16_t x, uint16_t y, uint8_t color);

/*
** Read a pixel out of the frame buffer. This represents a single pixel
**
** @param x coordinate on the screen
** @param y coordinate on the screen
** @return Pixel color at position. This is an index into the color palette
*/
uint8_t _vga13_get_pixel(uint16_t x, uint16_t y);

/*
** Draws a simple rectangle
**
** @param urx Upper-right x coordinate on the screen
** @param ury Upper-right y coordinate on the screen
** @param llx Lower-left x coordinate on the screen
** @param lly Lower-left y coordinate on the screen
** @param color Pixel color to write. This is an index into the color palette
*/
void _vga13_draw_rect(uint16_t urx, uint16_t ury, uint16_t llx, uint16_t lly, 
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
void _vga13_draw_rect_wh(uint16_t ulx, uint16_t uly, uint16_t w, uint16_t h,
    uint8_t color);

#endif
