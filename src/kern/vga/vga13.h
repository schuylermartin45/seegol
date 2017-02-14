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
// standard header for VGA drivers
#include "vga.h"

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
** Start VGA's Mode13h
**
** @param driver Standard driver spec for a driver to setup
** @param aux Auxiliary input data. This is sets the palette in Mode13h
*/
void _vga13_enter(VGA_Driver* driver, uint16_t aux);

#endif
