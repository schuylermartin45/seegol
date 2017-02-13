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

// remember what graphical mode we are in
static uint8_t vga_mode = GL_MODE_TEXT;
// address of start of the frame buffer for each mode
static uint16_t* frame_buff = (uint16_t*)TEXT_MEM_BEGIN;
// current screen size
static uint16_t screen_w = TEXT_WIDTH;
static uint16_t screen_h = TEXT_HEIGHT;

/*
** Enter a graphical mode
**
** @param mode Graphics mode to select
*/
void gl_lib_enter(uint8_t mode)
{
    // start handling text debugging in the back up buffer
    kio_swap_fb();
    // record what mode we are in
    vga_mode = mode;
    switch (vga_mode)
    {
        case GL_MODE_VGA13:
            frame_buff = (uint16_t*)VGA13_MEM_BEGIN;
            screen_w = VGA13_WIDTH;
            screen_h = VGA13_HEIGHT;
            _vga13_enter();
            break;
    }
}

/*
** Exit graphical mode and go back to text
*/
void gl_lib_exit(void)
{
    // clean-up after the mode that just ended
    gl_lib_clrscr();
    // reset to the default mode
    vga_mode = GL_MODE_TEXT;
    frame_buff = (uint16_t*)TEXT_MEM_BEGIN;
    screen_w = TEXT_WIDTH;
    screen_h = TEXT_HEIGHT;
    // punch it Chewie, make the jump back to text mode
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
uint16_t gl_lib_getw(void)
{
    return screen_w;
}

/*
** Get the current height of the screen
**
** @param Height of the screen in the current mode
*/
uint16_t gl_lib_geth(void)
{
    return screen_h;
}

/*
** Clears the video buffer
*/
void gl_lib_clrscr(void)
{
    switch (vga_mode)
    {
        case GL_MODE_VGA13:
            _vga13_clrscr();
            break;
    }
}

/* Draw functions */

/*
** Draws a pixel to the screen. Coordinates start in the upper-left corner
**
** @param x Pixel position in the x direction
** @param y Pixel position in the y direction
** @param color Pixel color to write. This is an index into the color palette
*/
void gl_lib_put_pixel(uint16_t x, uint16_t y, uint8_t color)
{
    switch (vga_mode)
    {
        case GL_MODE_VGA13:
            _vga13_put_pixel(x, y, color);
            break;
    }
}

/*
** Gets a pixel's color value. Coordinates start in the upper-left corner
**
** @param x Pixel position in the x direction
** @param y Pixel position in the y direction
** @return Pixel's color value
*/
uint8_t gl_lib_get_pixel(uint16_t x, uint16_t y)
{
    uint8_t color = 0x0;
    switch (vga_mode)
    {
        case GL_MODE_VGA13:
            color = _vga13_get_pixel(x, y);
            break;
    }
    return color;
}

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
    uint8_t color)
{
    switch (vga_mode)
    {
        case GL_MODE_VGA13:
            _vga13_draw_rect(urx, ury, llx, lly, color);
            break;
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
void gl_lib_draw_rect_wh(uint16_t ulx, uint16_t uly, uint16_t w, uint16_t h,
    uint8_t color)
{
    switch (vga_mode)
    {
        case GL_MODE_VGA13:
            _vga13_draw_rect_wh(ulx, uly, w, h, color);
            break;
    }
}
