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

// WARNING:
// Don't use ".code16". Doing so does not build the project correctly. It
// appears this directive gives gcc more context about the bootloading process
// and doesn't screw up the stack. Strange things occur when the other
// directive is used (stack space is exhausted very easily, parameters are not
// correctly passed, undeterministic output)
__asm__(".code16gcc\n");

#endif
