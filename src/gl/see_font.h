/*
** File:    see_font.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Defines a basic font for VGA graphical modes.
**              "Shoyler's Extremely Experimental Font"
*/
#ifndef _SEE_FONT_H_
#define _SEE_FONT_H_

/** Headers    **/
#include "../kern/gcc16.h"
#include "../kern/types.h"

/** Macros     **/
// font bit map is 8x8 in size (8 bytes total)
#define SEE_FONT_WIDTH      8
#define SEE_FONT_HEIGHT     8
// pixel padding, one side
#define SEE_FONT_PAD_VERT   2
#define SEE_FONT_PAD_HORZ   0
// font will hopefully suport 7-bit ASCII
#define SEE_FONT_NUM_CH     128

/** Globals    **/

// table of character bit maps
extern const uint8_t see_font_tbl[SEE_FONT_NUM_CH][SEE_FONT_HEIGHT];

#endif
