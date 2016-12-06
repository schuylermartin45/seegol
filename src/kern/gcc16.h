/*
** File:    gcc16.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Header file that should be included at the top of every file.
**              It ensures that C files are assembled in Real Mode, 16-bit x86
*/
#ifndef _GCC16_H_
#define _GCC16_H_

__asm__(".code16\n");
// TODO what's the difference? specific to gcc(?)
//__asm__(".code16gcc\n");

#endif
