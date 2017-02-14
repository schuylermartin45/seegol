/*
** File:    vga13.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Driver for VGA's "Mode 13h" graphics mode
*/

/** Headers    **/
#include "../gcc16.h"
#include "vga13.h"

/*
** Clears the video buffer
*/
static void __vga13_clrscr(void)
{
    uint16_t* addr = (uint16_t*)VGA13_MEM_BEGIN;
    while(addr < (uint16_t*)VGA13_MEM_END)
    {
        *addr++ = 0;
    }
}


/*
** Write a byte out to the frame buffer. This represents a single pixel
**
** @param addr Address of the pixel in the frame buffer to write to
** @param pxb Pixel byte to write. This is an index into the color palette
*/
static void __vga13_fbwb(uint8_t* addr, uint8_t pxb)
{
    *addr = pxb;
}

/*
** Read a byte out of the frame buffer. This represents a single pixel
**
** @param addr Address of the pixel in the frame buffer to write to
** @return Pixel's color value
*/
static uint8_t __vga13_fbrb(uint8_t* addr)
{
    return *addr;
}

/*
** Write a pixel out to the frame buffer. This represents a single pixel
**
** @param x coordinate on the screen
** @param y coordinate on the screen
** @param color Pixel color to write. This is an index into the color palette
*/
static void __vga13_put_pixel(uint16_t x, uint16_t y, uint8_t color)
{
    // calculate the pixel offset and set the pixel accordingly
    *((uint8_t*)(VGA13_MEM_BEGIN + ((y * VGA13_WIDTH) + x))) = color;
}

/*
** Read a pixel out of the frame buffer. This represents a single pixel
**
** @param x coordinate on the screen
** @param y coordinate on the screen
** @return Pixel color at position. This is an index into the color palette
*/
static uint8_t __vga13_get_pixel(uint16_t x, uint16_t y)
{
    return *((uint8_t*)(VGA13_MEM_BEGIN + ((y * VGA13_WIDTH) + x)));
}

/* "Fast" (i.e. lazy) drawing methods */

/*
** Draws a simple rectangle
**
** @param urx Upper-right x coordinate on the screen
** @param ury Upper-right y coordinate on the screen
** @param llx Lower-left x coordinate on the screen
** @param lly Lower-left y coordinate on the screen
** @param color Pixel color to write. This is an index into the color palette
*/
static void __vga13_draw_rect(uint16_t urx, uint16_t ury, uint16_t llx,
    uint16_t lly, uint8_t color)
{
    // we actually start drawing on the upper left pixel, so calculate the
    // address at that position first
    uint16_t* addr = (uint16_t*)(VGA13_MEM_BEGIN + ((ury * VGA13_WIDTH) + llx));
    // iterate in a smart fashion, reducing memory access

    // do a little preprocessing: how many times we can do chunk copies per
    // scanline. this reduces the number of memory writes we need to do as
    // these addresses are right next to each other
    uint8_t div = (urx - llx) / sizeof(uint16_t);
    uint8_t rem = (urx - llx) % sizeof(uint16_t);
    uint16_t packed_color = color;
    packed_color <<= 8; packed_color += color;

    // difference in address between end of current and start of next scanline
    uint16_t addr_diff = (VGA13_WIDTH - urx) + llx;

    // iterate over all scanlines
    for (uint16_t yi=0; yi<(lly - ury); yi++)
    {
        // draw the rectangles
        // iterate down the scanline
        for (uint16_t xi=0; xi<div; xi++)
        {
            *addr++ = packed_color;
        }
        // finish the remaining amount by writing bytes out
        for (uint16_t xi=0; xi<rem; xi++)
        {
            *((uint8_t*)addr) = color;
            addr++;
        }
        // advance the address to the start of the next scanline pixel to draw
        addr += (addr_diff / sizeof(uint16_t));
    }
}

/*
** Draws a simple rectangle, using alternative parameter listings
**
** @param ulx Upper-left x coordinate on the screen
** @param uly Upper-left y coordinate on the screen
** @param w Width of the rectangle
** @param h Height of the rectangle
** @param color Pixel color to write. This is an index into the color palette
*/
static void __vga13_draw_rect_wh(uint16_t ulx, uint16_t uly, uint16_t w,
    uint16_t h, uint8_t color)
{
    __vga13_draw_rect(ulx + w, uly, ulx, uly + h, color);
}

/*
** Start VGA's Mode13h
**
** @param driver Standard driver spec for a driver to setup
** @param aux Auxiliary input data. This is sets the palette in Mode13h
*/
void _vga13_enter(VGA_Driver* driver, uint16_t aux)
{
    // configure VGA13 mode
    driver->screen_w = VGA13_WIDTH;
    driver->screen_h = VGA13_HEIGHT;
    // address of start of the frame buffer for each mode
    driver->frame_buff = (uint16_t*)VGA13_MEM_BEGIN;
    // remember what graphical mode we are in
    driver->vga_mode = VGA_MODE_13;
    // set all the functions here
    driver->vga_enter = &_vga13_enter;
    driver->vga_clrscr = &__vga13_clrscr;
    driver->vga_fbwb = &__vga13_fbwb;
    driver->vga_fbrb = &__vga13_fbrb;
    driver->vga_put_pixel = &__vga13_put_pixel;
    driver->vga_get_pixel = &__vga13_get_pixel;
    driver->vga_draw_rect = &__vga13_draw_rect;
    driver->vga_draw_rect_wh = &__vga13_draw_rect_wh;

    // BIOS interrupt appears to clear memory on reset
    __asm__ __volatile__("movb $0x13, %al\n");
    __asm__ __volatile__("movb $0x00, %ah\n");
    __asm__ __volatile__("int  $0x10\n");
}

