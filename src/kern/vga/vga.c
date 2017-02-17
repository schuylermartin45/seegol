/*
** File:    vga.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Common VGA definitions and utilities
*/

/** Headers    **/
#include "gcc16.h"
#include "vga.h"

/** Globals    **/
// common colors
RGB_8 RGB_8_BLACK   = {  0,   0,   0};
RGB_8 RGB_8_WHITE   = {255, 255, 255};
// HSC logo color
RGB_8 RGB_8_HSC     = { 40,  49, 137};

/** Functions  **/

/*
** Compares two colors for equivalency
**
** @param c0 First color
** @param c1 Second color
** @param Color equivalency
*/
bool vga_RGB_8_cmp(RGB_8* c0, RGB_8* c1)
{
    if ((c0->r == c1->r) && (c0->g == c1->g) && (c0->b == c1->b))
        return true;
    return false;
}
