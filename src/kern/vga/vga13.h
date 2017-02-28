/*
** File:    vga13.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Driver for VGA's "Mode 13h" graphics mode
**              This driver is capable of modifying the color palette table
**              in Mode 13h, adding requested colors to the table on the fly
*/
#ifndef _VGA13_H_
#define _VGA13_H_

/** Headers    **/
#include "../gcc16.h"
#include "../types.h"
#include "../asm_lib.h"
// standard header for VGA drivers
#include "vga.h"

/** Macros     **/
// Graphics mode screen dimensions (1 byte per pixel in memory)
#define VGA13_WIDTH     320
#define VGA13_HEIGHT    200
// Start and ending address of the memory space that Mode13h uses (64kB total)
#define VGA13_MEM_SIZE   (VGA13_WIDTH * VGA13_HEIGHT)
#define VGA13_MEM_BEGIN  0xA0000
#define VGA13_MEM_END    (VGA13_MEM_BEGIN + VGA13_MEM_SIZE)

// size of Mode13h palette (color look-up table)
#define VGA13_PALETTE_SIZE      256
// reserved colors, and how many are reserved
#define VGA13_PALETTE_BLACK     0
#define VGA13_PALETTE_WHITE     (VGA13_PALETTE_SIZE - 1)
#define VGA13_PALETTE_RESERVED  2
// Error codes mapped to reserved colors, since we will never overwrite them
#define VGA13_PALETTE_NOT_FOUND VGA13_PALETTE_BLACK
// Port addresses for palette control; these are 
#define VGA13_PALETTE_PORT_IDX  0x03C8
#define VGA13_PALETTE_PORT_CLR  0x03C9

/** Globals    **/

/** Structures **/

/** Functions  **/

/*
** Start VGA's Mode13h
**
** @param driver Standard driver spec for a driver to setup
*/
void _vga13_enter(VGA_Driver* driver);

#endif
