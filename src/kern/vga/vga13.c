/*
** File:    vga13.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Driver for VGA's "Mode 13h" graphics mode
**              This driver is capable of modifying the color palette table
**              in Mode 13h, adding requested colors to the table on the fly
*/

/** Headers    **/
#include "../gcc16.h"
#include "../asm_lib.h"
#include "vga13.h"

// Palette look-up table in memory. Should be faster to use than Port I/O
// Reserved:
//   + Black (and Error code)
//   + White
static const RGB_8 RGB_8_BLACK = {  0,   0,   0};
static const RGB_8 RGB_8_WHITE = {255, 255, 255};
static RGB_8 color_palette[VGA13_PALETTE_SIZE];
// current index into the table
static uint8_t palette_idx;

/************************** Palette Functions **************************/

/*
** Sets the color value in the palette (look-up table) on the VGA controller
** using Port I/O
**
** @param idx Index of the value to set (0-255)
** @param color RGB color value to set
*/
static void __vga13_set_port_color(uint8_t idx, RGB_8 color)
{
    // look through the table
    // RGB values written to the device table/palette, under an index value
    _outb(VGA13_PALETTE_PORT_IDX, idx);
    // this mode actually only uses 6 bit per channel; 18bit not 24bit color
    // so right shifting by 2 bits will quantize the color space, giving a
    // closer approximation of the desired color
    _outb(VGA13_PALETTE_PORT_CLR, color.r >> 2);
    _outb(VGA13_PALETTE_PORT_CLR, color.g >> 2);
    _outb(VGA13_PALETTE_PORT_CLR, color.b >> 2);
}

/*
** Fetchs a color in the VGA13 palette. If a match is not found, the color
** is added to the table for future use
**
** @param color RGB Color to set
** @return Look up value in the color palette table. To write to frame buffer
*/
static uint8_t __vga13_fetch_color(RGB_8 color)
{
    // check reserved colors; prevent modification to the palette
    if (vga_RGB_8_cmp(color, color_palette[VGA13_PALETTE_BLACK]))
        return VGA13_PALETTE_BLACK;
    if (vga_RGB_8_cmp(color, color_palette[VGA13_PALETTE_WHITE]))
        return VGA13_PALETTE_WHITE;
    // perform a search to prevent duplicates
    uint8_t color_code = VGA13_PALETTE_NOT_FOUND;
    // For now we will perform a linear search and hope that is fast enough.
    // Ideally the draw functions should only need to call this once. Commonly
    // used colors should be "cached" towards the front of the color table;
    // Only loops until the last color set; prevents wasted search time
    for (uint8_t i=VGA13_PALETTE_BLACK + 1; i<palette_idx; ++i)
    {
        if (vga_RGB_8_cmp(color_palette[i], color))
        {
            color_code = i;
            break;
        }
    }
    // unfound colors are added to the table
    if (color_code == VGA13_PALETTE_NOT_FOUND)
    {
        // move to the next palette entry, staying within bounds
        // chances are that all 256 colors will never be fully used in this OS
        // As a side note, CGA (PCjr) had built-in color palettes
        __vga13_set_port_color(palette_idx, color);
        // add color to the table
        color_code = palette_idx;
        color_palette[color_code] = color;
        palette_idx %= (VGA13_PALETTE_SIZE - VGA13_PALETTE_RESERVED);
        ++palette_idx;
    }
    return color_code;
}

/*
** Initializes the color palette
*/
static void __vga13_init_color()
{
    // mimmic these changes in our memory mapping
    color_palette[VGA13_PALETTE_BLACK] = RGB_8_BLACK;
    color_palette[VGA13_PALETTE_WHITE] = RGB_8_WHITE;
    // set black and white values
    __vga13_set_port_color(VGA13_PALETTE_BLACK, RGB_8_BLACK);
    __vga13_set_port_color(VGA13_PALETTE_WHITE, RGB_8_WHITE);
    // valid range: Black + 1 to White - 1
    palette_idx = VGA13_PALETTE_BLACK + 1;
}

/************************** Draw Functions **************************/

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
** Vertical sync control. Useful for slow electron-gun-based displays
*/
static void __vga13_vsync(void)
{
    // bit 3 indicates whether or not the display has finished tracing pixels
    // wait for a previous retrace to end
    while (_inb(VGA13_VSYNC_PORT) & 0b100) {};
    // wait for a new retrace to begin
    while (!(_inb(VGA13_VSYNC_PORT) & 0b100)) {};
}

/*
** Write a pixel out to the frame buffer. This represents a single pixel
**
** @param x coordinate on the screen
** @param y coordinate on the screen
** @param color Pixel color to write. This is an index into the color palette
*/
static void __vga13_put_pixel(uint16_t x, uint16_t y, RGB_8 color)
{
    uint8_t color_code = __vga13_fetch_color(color);
    // calculate the pixel offset and set the pixel accordingly
    *((uint8_t*)(VGA13_MEM_BEGIN + ((y * VGA13_WIDTH) + x))) = color_code;
}

/*
** Read a pixel out of the frame buffer. This represents a single pixel
**
** @param x coordinate on the screen
** @param y coordinate on the screen
** @param color Pixel color written to the pixel position
*/
static void __vga13_get_pixel(uint16_t x, uint16_t y, RGB_8* color)
{
    uint8_t color_code =
        *((uint8_t*)(VGA13_MEM_BEGIN + ((y * VGA13_WIDTH) + x)));
    // look up color in the table and set it
    *color = color_palette[color_code];
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
    uint16_t lly, RGB_8 color)
{
    uint8_t color_code = __vga13_fetch_color(color);
    // we actually start drawing on the upper left pixel, so calculate the
    // address at that position first
    uint16_t* addr = (uint16_t*)(VGA13_MEM_BEGIN + ((ury * VGA13_WIDTH) + llx));
    // iterate in a smart fashion, reducing memory access

    // do a little preprocessing: how many times we can do chunk copies per
    // scanline. this reduces the number of memory writes we need to do as
    // these addresses are right next to each other
    uint8_t div = (urx - llx) / sizeof(uint16_t);
    uint8_t rem = (urx - llx) % sizeof(uint16_t);
    uint16_t packed_color = color_code;
    packed_color <<= 8; packed_color += color_code;

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
            *((uint8_t*)addr) = color_code;
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
    uint16_t h, RGB_8 color)
{
    __vga13_draw_rect(ulx + w, uly, ulx, uly + h, color);
}

/************************** GL-Visible Functions **************************/

/*
** Start VGA's Mode13h
**
** @param driver Standard driver spec for a driver to setup
*/
void _vga13_enter(VGA_Driver* driver)
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
    driver->vga_vsync = &__vga13_vsync;
    driver->vga_put_pixel = &__vga13_put_pixel;
    driver->vga_get_pixel = &__vga13_get_pixel;
    driver->vga_draw_rect = &__vga13_draw_rect;
    driver->vga_draw_rect_wh = &__vga13_draw_rect_wh;

    // BIOS interrupt appears to clear memory on reset
    __asm__ __volatile__("movb $0x13, %al\n");
    __asm__ __volatile__("movb $0x00, %ah\n");
    __asm__ __volatile__("int  $0x10\n");
    // prep the color palette system. This should be a sufficient reset so that
    // a previous program's color requests don't interfere with the current's
    __vga13_init_color();
}

